/*
 * LameNES - Nintendo Entertainment System (NES) emulator
 *
 * Copyright (c) 2005, Joey Loman, <joey@lamenes.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "lame6502/lame6502.h"
#include "lame6502/debugger.h"

#include "lamenes.h"
#include "input.h"

#if defined(ndlib)
	#include <n2DLib.h>
#endif

SDL_Surface *screen;
SDL_Event event;

void
screen_lock()
{
	#if !defined(ndlib)
        if(SDL_MUSTLOCK(screen)) {
                if(SDL_LockSurface(screen) < 0) {
                        return;
                }
        }
    #endif
}

void
screen_unlock()
{
	#if !defined(ndlib)
        if(SDL_MUSTLOCK(screen)) {
                SDL_UnlockSurface(screen);
        }
    #endif
}

void
init_SDL(int type, int fullscreen)
{
	
#if defined(ndlib)

	SDL_Init(SDL_INIT_VIDEO);

	initBuffering();
	clearBufferB();
	updateScreen();
	
#else

#ifndef NSPIRE
	atexit(SDL_Quit);
#endif

	/* Need to be initialised, even for controls ! */
	if(SDL_Init(SDL_INIT_VIDEO) < 0) 
	{
		printf("error: unable to init SDL %s\n",SDL_GetError());
		exit(1);
	}
	
#ifdef NSPIRE

	if(type == 0) 
	{
		screen = SDL_SetVideoMode(sdl_screen_width, sdl_screen_height, has_colors ? 16 : 8, SDL_SWSURFACE);
	}
	else
	if(type == 1) 
	{
		screen = SDL_SetVideoMode(sdl_screen_width, sdl_screen_height, has_colors ? 16 : 8, SDL_SWSURFACE);
	}
	
	SDL_FillRect(screen, NULL, 0);
	SDL_Flip(screen);
#else
	/* PAL */
	if(type == 0) {
		if(fullscreen == 1) {
			screen=SDL_SetVideoMode(320, sdl_screen_height, 16, SDL_SWSURFACE | SDL_FULLSCREEN);
		} else {
			screen=SDL_SetVideoMode(320, sdl_screen_height, 16, SDL_SWSURFACE);
		}
	}

	/* NTSC */
	if(type == 1) {
		if(fullscreen == 1) {
			screen=SDL_SetVideoMode(320, sdl_screen_height, 16, SDL_SWSURFACE | SDL_FULLSCREEN);
		} else {
			screen=SDL_SetVideoMode(320, sdl_screen_height, 16, SDL_SWSURFACE);
		}
	}
#endif

	if(screen == NULL) {
		printf("error: unable to set video %s\n",SDL_GetError());
		exit(1);
	}


#endif

	return;
}

void
check_SDL_event()
{
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_KEYDOWN) {
			switch(event.key.keysym.sym) {
				case SDLK_DOWN:
				#ifdef PAD_DEBUG
				printf("debug [%d]: DOWN key (down) press detected\n",debug_cnt);
				#endif
				set_input((char *) 1);
				break;

				case SDLK_UP:
				#ifdef PAD_DEBUG
				printf("debug [%d]: UP key press (down) detected\n",debug_cnt);
				#endif
				set_input((char *) 2);
				break;

				case SDLK_LEFT:
				#ifdef PAD_DEBUG
				printf("debug [%d]: LEFT key press (down) detected\n",debug_cnt);
				#endif
				set_input((char *) 3);
				break;

				case SDLK_RIGHT:
				#ifdef PAD_DEBUG
				printf("debug [%d]: RIGHT key press (down) detected\n",debug_cnt);
				#endif
				set_input((char *) 4);
				break;

#ifdef NSPIRE
				case SDLK_MENU:
#else
				case SDLK_LCTRL:
#endif
				#ifdef PAD_DEBUG
				printf("debug [%d]: LCTRL (NES -> start) key (down) press detected\n",debug_cnt);
				#endif
				set_input((char *) 5);
				break;

#ifdef NSPIRE
				case SDLK_BACKSPACE:
#else
				case SDLK_LSHIFT:
#endif
				#ifdef PAD_DEBUG
				printf("debug [%d]: LSHIFT (NES -> select) key (down) press detected\n",debug_cnt);
				#endif
				set_input((char *) 6);
				break;

				case SDLK_p:
				if(pause_emulation) {
					printf("[*] LameNES continue emulation!\n");
					CPU_is_running = 1;
					pause_emulation = 0;
				} else if(!pause_emulation) {
					printf("[*] LameNES paused!\n");
					CPU_is_running = 0;
					pause_emulation = 1;
				}
				break;

#ifdef NSPIRE
				case SDLK_LSHIFT:
#else
				case SDLK_x:
#endif
				#ifdef PAD_DEBUG
				printf("debug [%d]: Z (NES -> A) key (down) press detected\n",debug_cnt);
				#endif
				set_input((char *) 7);
				break;

#ifdef NSPIRE
				case SDLK_LCTRL:
#else
				case SDLK_z:
#endif
				#ifdef PAD_DEBUG
				printf("debug [%d]: X (NES -> B) key (down) press detected\n",debug_cnt);
				#endif
				set_input((char *) 8);
				break;

				case SDLK_q:
				#ifdef PAD_DEBUG
				printf("debug [%d]: Q (quit lamenes) key (down) press detected\n",debug_cnt);
				#endif
				/*quit_emulation();*/
				break;

				case SDLK_ESCAPE:
				#ifdef PAD_DEBUG
				printf("debug [%d]: Q (quit lamenes) key (down) press detected\n",debug_cnt);
				#endif
				quit_emulation();
				break;

				case SDLK_F1:
				/* reset */
				reset_emulation();
				break;

#ifdef NSPIRE
				case SDLK_l:
#else
				case SDLK_F3:
#endif
				/* load state */
				load_state();
				break;

#ifdef NSPIRE
				case SDLK_s:
#else
				case SDLK_F6:
#endif
				/* save state */
				save_state();
				break;
			
#ifdef NSPIRE
				case SDLK_a:
#else
				case SDLK_F10:
#endif
				if(enable_background == 1) {
					enable_background = 0;
				} else {
					enable_background = 1;
				}
				break;

#ifdef NSPIRE
				case SDLK_b:
#else
				case SDLK_F11:
#endif
				if(enable_sprites == 1) {
					enable_sprites = 0;
				} else {
					enable_sprites = 1;
				}
				break;

				case SDLK_F12:
				if(startdebugger > 0) {
					disassemble = 1;
					hit_break = 1;
					debugger();
				}
				break;

				default:
				break;
			}
		}

		if(event.type == SDL_KEYUP) {
			switch(event.key.keysym.sym){
				case SDLK_DOWN:
				#ifdef PAD_DEBUG
				printf("debug [%d]: DOWN key (up) press detected\n",debug_cnt);
				#endif
				clear_input((char *) 1);
				break;

				case SDLK_UP:
				#ifdef PAD_DEBUG
				printf("debug [%d]: UP key (up) press detected\n",debug_cnt);
				#endif
				clear_input((char *) 2);
				break;

				case SDLK_LEFT:
				#ifdef PAD_DEBUG
				printf("debug [%d]: LEFT key (up) press detected\n",debug_cnt);
				#endif
				clear_input((char *) 3);
				break;

				case SDLK_RIGHT:
				#ifdef PAD_DEBUG
				printf("debug [%d]: RIGHT key (up) press detected\n",debug_cnt);
				#endif
				clear_input((char *) 4);
				break;

#ifdef NSPIRE
				case SDLK_MENU:
#else
				case SDLK_LCTRL:
#endif
				#ifdef PAD_DEBUG
				printf("debug [%d]: LCTRL (NES -> start) key (up) press detected\n",debug_cnt);
				#endif
				clear_input((char *) 5);
				break;

#ifdef NSPIRE
				case SDLK_BACKSPACE:
#else
				case SDLK_LSHIFT:
#endif
				#ifdef PAD_DEBUG
				printf("debug [%d]: LSHIFT (NES -> select) key (up) press detected\n",debug_cnt);
				#endif
				clear_input((char *) 6);
				break;

#ifdef NSPIRE
				case SDLK_LSHIFT:
#else
				case SDLK_x:
#endif
				#ifdef PAD_DEBUG
				printf("debug [%d]: Z (NES -> A) key (up) press detected\n",debug_cnt);
				#endif
				clear_input((char *) 7);
				break;

#ifdef NSPIRE
				case SDLK_LCTRL:
#else
				case SDLK_z:
#endif
				#ifdef PAD_DEBUG
				printf("debug [%d]: X (NES -> B) key (up) press detected\n",debug_cnt);
				#endif
				clear_input((char *) 8);
				break;

				default:
				break;
			}
		}
	}
}
