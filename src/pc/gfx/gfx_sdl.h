#ifndef GFX_SDL_H
#define GFX_SDL_H

#include "gfx_window_manager_api.h"

#ifdef __OBJC__
#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#endif

extern struct GfxWindowManagerAPI gfx_sdl;
#ifdef __OBJC__
UIViewController *get_sdl_viewcontroller();
#endif
static SDL_Window *wnd;

#endif
