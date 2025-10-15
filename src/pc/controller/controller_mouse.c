#include "controller_mouse.h"
#include "pc/gfx/gfx_pc.h"
#include "pc/djui/djui.h"

#ifdef WAPI_DXGI
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    extern HWND gfx_dxgi_get_h_wnd(void);
    static bool mouse_relative_prev_cursor_state;
#elif defined(CAPI_SDL1)
    #include <SDL/SDL.h>
#elif defined(CAPI_SDL2)
    #include <SDL2/SDL.h>
#endif

bool mouse_init_ok = true;

u32 mouse_buttons = 0;
s32 mouse_x = 0;
s32 mouse_y = 0;

u32 mouse_window_buttons = 0;
u32 mouse_window_buttons_pressed = 0;
u32 mouse_window_buttons_released = 0;
s32 mouse_window_x = 0;
s32 mouse_window_y = 0;

u32 mouse_scroll_timestamp = 0;
f32 mouse_scroll_x = 0.0f;
f32 mouse_scroll_y = 0.0f;

bool mouse_relative_enabled = false;

#ifdef TOUCH_CONTROLS
    typedef struct {
        bool active;
        float x;
        float y;
        s32 window_x;
        s32 window_y;
        SDL_FingerID id;
    } TouchState;

    static TouchState touch_window_state = {0};
    static u32 prev_touch_window_buttons = 0;

    typedef struct {
        bool tracking;
        float start_x;
        float start_y;
        s32 last_window_x;
        s32 last_window_y;
        s32 offset_x;
        s32 offset_y;
        SDL_FingerID tracked_id;
    } TouchRelativeState;

    static TouchRelativeState touch_relative_state = {0};

    static void update_touch_buttons(u32* current, u32* pressed, u32* released, bool is_active) {
        u32 old_current = *current;
        u32 new_button_state = is_active ? L_MOUSE_BUTTON : 0; // Сенсор = левая кнопка мыши

        *pressed = new_button_state & ~old_current;
        *released = ~new_button_state & old_current;
        *current = new_button_state;
    }

    static void normalize_to_window(float norm_x, float norm_y, s32* win_x, s32* win_y) {
        if (gfx_current_dimensions.width > 0 && gfx_current_dimensions.height > 0) {
            *win_x = (s32)(norm_x * gfx_current_dimensions.width);
            *win_y = (s32)(norm_y * gfx_current_dimensions.height);
            *win_x -= gfx_current_dimensions.x_adjust_4by3;
        } else {
            *win_x = 0;
            *win_y = 0;
        }
    }
#endif // TOUCH_CONTROLS

#ifdef TOUCH_CONTROLS
void handle_touch_down(SDL_TouchFingerEvent *event) {
    if (!touch_window_state.active) {
        touch_window_state.active = true;
        touch_window_state.x = event->x;
        touch_window_state.y = event->y;
        touch_window_state.id = event->fingerId;

        normalize_to_window(touch_window_state.x, touch_window_state.y, &touch_window_state.window_x, &touch_window_state.window_y);

        if (mouse_relative_enabled && !touch_relative_state.tracking) {
            touch_relative_state.start_x = touch_window_state.x;
            touch_relative_state.start_y = touch_window_state.y;
            touch_relative_state.last_window_x = touch_window_state.window_x;
            touch_relative_state.last_window_y = touch_window_state.window_y;
            touch_relative_state.tracking = true;
            touch_relative_state.tracked_id = touch_window_state.id;
        }
    }
}

void handle_touch_up(SDL_TouchFingerEvent *event) {
    if (touch_window_state.active && touch_window_state.id == event->fingerId) {
        touch_window_state.active = false;
        if (touch_relative_state.tracking && touch_relative_state.tracked_id == event->fingerId) {
            touch_relative_state.tracking = false;
            touch_relative_state.tracked_id = 0;
        }
        touch_window_state.id = 0;
    }
}

void handle_touch_motion(SDL_TouchFingerEvent *event) {
    if (touch_window_state.active && touch_window_state.id == event->fingerId) {
        touch_window_state.x = event->x;
        touch_window_state.y = event->y;

        normalize_to_window(touch_window_state.x, touch_window_state.y, &touch_window_state.window_x, &touch_window_state.window_y);
    }
}
#endif // TOUCH_CONTROLS


#ifdef WAPI_DXGI
u32 mouse_relative_buttons_held_on_focus;
u32 mouse_window_buttons_held_on_focus;
bool mouse_dxgi_prev_focus;

static u32 controller_mouse_dxgi_button_state(u32* mouse_held, bool has_focus) {
    u32 mouse =
        ((GetKeyState(VK_LBUTTON ) < 0) ? L_MOUSE_BUTTON : 0) |
        ((GetKeyState(VK_MBUTTON ) < 0) ? M_MOUSE_BUTTON : 0) |
        ((GetKeyState(VK_RBUTTON ) < 0) ? R_MOUSE_BUTTON : 0) |
        ((GetKeyState(VK_XBUTTON1) < 0) ? MOUSE_BUTTON_4 : 0) |
        ((GetKeyState(VK_XBUTTON2) < 0) ? MOUSE_BUTTON_5 : 0);

    bool prev_focus = mouse_dxgi_prev_focus;
    mouse_dxgi_prev_focus = has_focus;

    if (!has_focus) { return 0; }
    if (mouse_held == NULL) { return mouse; }

    *mouse_held = (*mouse_held) & mouse;
    return ~(*mouse_held) & mouse;
}
#endif // WAPI_DXGI


void controller_mouse_read_window(void) {
#ifndef TOUCH_CONTROLS
    #if defined(WAPI_DXGI)
        HWND game_window = gfx_dxgi_get_h_wnd();

        mouse_window_buttons = controller_mouse_dxgi_button_state(
            &mouse_window_buttons_held_on_focus,
            GetFocus() == game_window);

        POINT p;
        if (GetCursorPos(&p) && ScreenToClient(game_window, &p)) {
            mouse_window_x = p.x - gfx_current_dimensions.x_adjust_4by3;
            mouse_window_y = p.y;
        }
    #elif defined(CAPI_SDL1) || defined(CAPI_SDL2)
        mouse_window_buttons = SDL_GetMouseState(&mouse_window_x, &mouse_window_y);
        mouse_window_x -= gfx_current_dimensions.x_adjust_4by3;
    #endif
#else
    mouse_window_x = touch_window_state.window_x;
    mouse_window_y = touch_window_state.window_y;

    update_touch_buttons(&mouse_window_buttons, &mouse_window_buttons_pressed, &mouse_window_buttons_released, touch_window_state.active);
    prev_touch_window_buttons = mouse_window_buttons;
#endif // TOUCH_CONTROLS
}


void controller_mouse_read_relative(void) {
#ifndef TOUCH_CONTROLS
    #if defined(WAPI_DXGI)
        HWND game_window = gfx_dxgi_get_h_wnd();

        mouse_buttons = controller_mouse_dxgi_button_state(
            &mouse_relative_buttons_held_on_focus,
            GetFocus() == game_window);

        if (mouse_relative_enabled) {
            static POINT p0;
            POINT p1;
            RECT rect;
            if (GetWindowRect(game_window, &rect) && GetCursorPos(&p1)) {
                mouse_x = p1.x - p0.x;
                mouse_y = p1.y - p0.y;

                p0.x = rect.left + (rect.right - rect.left) / 2;
                p0.y = rect.top + (rect.bottom - rect.top) / 2;
                SetCursorPos(p0.x, p0.y);
            }
        } else {
            mouse_x = 0;
            mouse_y = 0;
        }

    #elif defined(CAPI_SDL1) || defined(CAPI_SDL2)
        mouse_buttons = SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
    #endif
#else
    mouse_x = 0;
    mouse_y = 0;
    mouse_buttons = 0;

    if (mouse_relative_enabled && touch_relative_state.tracking) {
        s32 current_window_x, current_window_y;
        normalize_to_window(touch_window_state.x, touch_window_state.y, &current_window_x, &current_window_y);

        mouse_x = current_window_x - touch_relative_state.last_window_x;
        mouse_y = current_window_y - touch_relative_state.last_window_y;

        touch_relative_state.last_window_x = current_window_x;
        touch_relative_state.last_window_y = current_window_y;

        mouse_buttons = mouse_window_buttons;
    }
#endif // TOUCH_CONTROLS
}


void controller_mouse_enter_relative(void) {
    if (!mouse_relative_enabled) {
        mouse_relative_enabled = true;

#ifndef TOUCH_CONTROLS
    #if defined(WAPI_DXGI)
        CURSORINFO ci;
        ci.cbSize = sizeof(CURSORINFO);
        if (GetCursorInfo(&ci)) {
            mouse_relative_prev_cursor_state = (0 != ci.flags);
        } else {
            mouse_relative_prev_cursor_state = false;
        }
        ShowCursor(FALSE);
    #elif defined(CAPI_SDL1)
        SDL_WM_GrabInput(SDL_GRAB_ON);
    #elif defined(CAPI_SDL2)
        SDL_SetRelativeMouseMode(SDL_TRUE);
    #endif
#else
        if (touch_window_state.active && !touch_relative_state.tracking) {
             s32 temp_x, temp_y;
             normalize_to_window(touch_window_state.x, touch_window_state.y, &temp_x, &temp_y);
             touch_relative_state.start_x = touch_window_state.x;
             touch_relative_state.start_y = touch_window_state.y;
             touch_relative_state.last_window_x = temp_x;
             touch_relative_state.last_window_y = temp_y;
             touch_relative_state.tracking = true;
             touch_relative_state.tracked_id = touch_window_state.id;
        }
#endif // TOUCH_CONTROLS
    }
}


void controller_mouse_leave_relative(void) {
    if (mouse_relative_enabled) {
        mouse_relative_enabled = false;

#ifndef TOUCH_CONTROLS
    #if defined(WAPI_DXGI)
        ShowCursor(mouse_relative_prev_cursor_state);
    #elif defined(CAPI_SDL1)
        SDL_WM_GrabInput(SDL_GRAB_OFF);
    #elif defined(CAPI_SDL2)
        SDL_SetRelativeMouseMode(SDL_FALSE);
    #endif
#else
        touch_relative_state.tracking = false;
        touch_relative_state.tracked_id = 0;
#endif // TOUCH_CONTROLS
    }
}


void mouse_on_scroll(float x, float y) {
    djui_interactable_on_scroll(x, y);
    mouse_scroll_timestamp = gGlobalTimer;
    mouse_scroll_x += x;
    mouse_scroll_y += y;
}
