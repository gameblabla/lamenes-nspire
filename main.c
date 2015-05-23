/*  
  COPYRIGHT (C) 2015, Bruno Marie 
  (aka Gameblabla)
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 3
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/*
 * nFile Browser 
 * Simple file browser mainly targetting the TI nspire using SDL.
 * Can also be used on other platforms as a simple file browser.
 * On TI Nspire, it can launch Ndless applications.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef ndlib
	#include <SDL/SDL.h>
#else
	#include "n2DLib.h"
#endif

#ifdef NSPIRE
	#include <nucleus.h>
	#include <libndls.h>
#endif

#include "data.h"
#include "main.h"

#ifdef ndlib
	struct rectangle
	{
		unsigned short x;
		unsigned short y;
		unsigned short w;
		unsigned short h;
	} gui_dst, gui_dst2;
#else
	SDL_Surface *gui_screen;
	SDL_Rect gui_dst, gui_dst2;
	SDL_Event gui_event;
#endif

char file_name[MAX_LENGH][1024];
short file_type[MAX_LENGH];

char* currentdir;
char* last_folder;

/* Maybe i should just use a structure instead here ?*/
unsigned char esc, up_button, down_button, ctrl_button, backspace_button;

unsigned char UP_state, UP_time;
unsigned char DOWN_state, DOWN_time;
unsigned char ESC_state, ESC_time;
unsigned char CTRL_state, CTRL_time;
unsigned char BACKSPACE_state, BACKSPACE_time;

/* This was done in order to save some memory on limited platforms*/
#if MAX_LENGH < 255
	unsigned char fileid_selected;
	unsigned char choice;
	unsigned char scroll_choice;
	unsigned char numb_files;
#elif MAX_LENGH < 65000
	unsigned short fileid_selected;
	unsigned short choice;
	unsigned short scroll_choice;
	unsigned short numb_files;
#else
	unsigned long fileid_selected;
	unsigned long choice;
	unsigned long scroll_choice;
	unsigned long numb_files;
#endif

unsigned char delete_file_screen();
void clear_entirescreen();
void update_entirescreen();


void init(void) 
{
#ifdef ndlib
	initBuffering();
	clearBufferB();
	updateScreen();
#else
	SDL_Init(SDL_INIT_VIDEO);
	#ifdef NSPIRE
		gui_screen = SDL_SetVideoMode(320, 240, has_colors ? 16 : 8, SDL_SWSURFACE ); 
	#else
		gui_screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE ); 
	#endif
	
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("LameNES - Choose NES ROM", NULL);
#endif
	
	up_button = 0;
	down_button = 0;
	esc = 0;
	ctrl_button = 0;

    /* Set the size of the clearing area. x, w, h never change so they are set here */	
	gui_dst.x = 16;
	gui_dst.w = 16;
	gui_dst.h = 48;
	
    /* Used to define the whole screen to be cleared */	
	gui_dst2.x = 0;
	gui_dst2.y = 0;
	gui_dst2.w = 320;
	gui_dst2.h = 240;
	
	choice = 0;
	
	/* scroll_choice = 0 means that files from id 0 to 11 (file_name) will be shown on screen  */	
	scroll_choice = 0;
}

int main(int argc, char* argv[]) 
{
	/* Buffer to hold =>the current directory */
	char* buf = NULL;
	unsigned char exit_app = 0;
#ifdef EXECUTE_APP
	unsigned char file_chosen;
	unsigned char done = 1;
#endif
	
	/* Temporary array, used for starting executable*/
	char final[MAX_LENGH];
	
	/* Init video and variables */
	init();
	
	clear_entirescreen();
	
	/* Set it to the current directory. */
	currentdir = getcwd(buf, MAX_LENGH);
	
	/* List 12 files to be shown on screen (here, it is the first chunck) */
	list_all_files(currentdir);
	
	update_entirescreen();
	
	/* Refresh everything on screen */
	refresh_cursor(1);
	
	while (exit_app==0) 
	{
		while ((ESC_state<1 || ESC_state>1))
		{
			/* Call function for input */
			controls();
			
				/* If Up button is pressed down... */
				if (UP_state == 1)
				{
					if (choice > 0) 
					{
						choice--;
						refresh_cursor(0);
						set_fileid();
					}
					else if (scroll_choice > 0)
					{
						choice = 11;
						scroll_choice = scroll_choice - 1;
						refresh_cursor(3);
						set_fileid();
					}
				}
				/* If Down button is pressed down... */
				else if (DOWN_state == 1)
				{
					/* Don't let the user to scroll more than there are files... */
					if (fileid_selected < numb_files)
					{
						if (choice < 11) 
						{
							choice++;	
							refresh_cursor(0);
							set_fileid();
						}
						/* If the user wants to go down and there are more files, change the files to shown to the next one (thanks to scroll_choice) */
						else if (numb_files > 10)
						{
							scroll_choice = scroll_choice + 1;
							choice = 0;
							set_fileid();
							refresh_cursor(3);
						}
					}
				}
			
				if (ESC_state == 1)
				{
#ifdef EXECUTE_APP
					file_chosen = 0;
#endif
					exit_app = 1;
				}
			
			
				/* If Control button is pressed... */
				if (CTRL_state == 1)
				{
					/* If file is a tns file then launch it */
					if (file_type[fileid_selected] == BLUE_C) 
					{
						sprintf(final, "%s/%s", currentdir, file_name[fileid_selected]);
						#ifdef EXECUTE_APP
							file_chosen = 1;
							ESC_state = 1;
						#else
							#ifdef NSPIRE
								nl_exec(final, 0, NULL);
							#endif	
						#endif
					}
					/* If not then it is a folder, if thats the case then go to that folder */
					else if (file_type[fileid_selected] == F_C || choice == 0) 
					{
						goto_folder();
					}
				
				}
			
			/* Don't waste CPU cycles */
			#ifndef ndlib
				SDL_Delay(16);
			#endif
		}
	
#ifdef EXECUTE_APP
		if (file_chosen == 1)
		{
			#ifdef ndlib
				deinitBuffering();
			#else
				if (gui_screen != NULL) SDL_FreeSurface(gui_screen);
				SDL_QuitSubSystem(SDL_INIT_VIDEO);
			#endif
				
			while(done == 1)
				{
					lamenes(final);
					done = 0;
				}
				
				done = 1;
				file_chosen = 0;
				ESC_state = 2;
				ESC_time = 0;
				init();
				clear_entirescreen();
				currentdir = getcwd(buf, MAX_LENGH);
				list_all_files(currentdir);
				update_entirescreen();
				refresh_cursor(2);
		}
		else
		{
				exit_app = 1;
		}
#endif
	}


#ifdef ndlib
	clearBufferB();
	deinitBuffering();
#else
	if (gui_screen != NULL) SDL_FreeSurface(gui_screen);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
#endif

	exit(0);
}

void controls()
{
#ifndef ndlib
    Uint8 *keystate = SDL_GetKeyState(NULL);
#endif
 
/*	Pressure buttons
 *  0 means Inactive
 *  1 means that the button was just pressed
 *  2 means that the button is being held
 *  3 means RELEASE THE BOGUS
*/

    switch (UP_state)
    {
		case 0:
			if (up_button)
			{
				UP_state = 1;
				UP_time = 0;
			}
		break;
		
		case 1:
			UP_time = UP_time + 1;
			
			if (UP_time > 0)
			{
				UP_state = 2;
				UP_time = 0;
			}
		break;
		
		case 2:
			if (!(up_button))
			{
				UP_state = 3;
				UP_time = 0;
			}
		break;
		
		case 3:
			UP_time = UP_time + 1;
			
			if (UP_time > 1)
			{
				UP_state = 0;
				UP_time = 0;
			}
		break;
	}

    switch (DOWN_state)
    {
		case 0:
			if (down_button)
			{
				DOWN_state = 1;
				DOWN_time = 0;
			}
		break;
		
		case 1:
			DOWN_time = DOWN_time + 1;
			
			if (DOWN_time > 0)
			{
				DOWN_state = 2;
				DOWN_time = 0;
			}
		break;
		
		case 2:
			if (!(down_button))
			{
				DOWN_state = 3;
				DOWN_time = 0;
			}
		break;
		
		case 3:
			DOWN_time = DOWN_time + 1;
			
			if (DOWN_time > 1)
			{
				DOWN_state = 0;
				DOWN_time = 0;
			}
		break;
	}

    switch (CTRL_state)
    {
		case 0:
			if (ctrl_button)
			{
				CTRL_state = 1;
				CTRL_time = 0;
			}
		break;
		
		case 1:
			CTRL_time = CTRL_time + 1;
			
			if (CTRL_time > 0)
			{
				CTRL_state = 2;
				CTRL_time = 0;
			}
		break;
		
		case 2:
			if (!(ctrl_button))
			{
				CTRL_state = 3;
				CTRL_time = 0;
			}
		break;
		
		case 3:
			CTRL_time = CTRL_time + 1;
			
			if (CTRL_time > 1)
			{
				CTRL_state = 0;
				CTRL_time = 0;
			}
		break;
	}

    switch (ESC_state)
    {
		case 0:
			if (esc)
			{
				ESC_state = 1;
				ESC_time = 0;
			}
		break;
		
		case 1:
			ESC_time = ESC_time + 1;
			
			if (ESC_time > 0)
			{
				ESC_state = 2;
				ESC_time = 0;
			}
		break;
		
		case 2:
			if (!(esc))
			{
				ESC_state = 3;
				ESC_time = 0;
			}
		break;
		
		case 3:
			ESC_time = ESC_time + 1;
			
			if (ESC_time > 1)
			{
				ESC_state = 0;
				ESC_time = 0;
			}
		break;
	}

    switch (BACKSPACE_state)
    {
		case 0:
			if (backspace_button)
			{
				BACKSPACE_state = 1;
				BACKSPACE_time = 0;
			}
		break;
		
		case 1:
			BACKSPACE_time = BACKSPACE_time + 1;
			
			if (BACKSPACE_time > 0)
			{
				BACKSPACE_state = 2;
				BACKSPACE_time = 0;
			}
		break;
		
		case 2:
			if (!(backspace_button))
			{
				BACKSPACE_state = 3;
				BACKSPACE_time = 0;
			}
		break;
		
		case 3:
			BACKSPACE_time = BACKSPACE_time + 1;
			
			if (BACKSPACE_time > 1)
			{
				BACKSPACE_state = 0;
				BACKSPACE_time = 0;
			}
		break;
	}


#ifdef ndlib
	if (isKeyPressed(KEY_NSPIRE_UP))	
#else
	if (keystate[SDLK_UP])
#endif
	{
		up_button = 1;
	}
	else
	{
		up_button = 0;
	}
	
#ifdef ndlib
	if (isKeyPressed(KEY_NSPIRE_DOWN))	
#else
	if (keystate[SDLK_DOWN])
#endif
	{
		down_button = 1;
	}
	else
	{
		down_button = 0;
	}
	
#ifdef ndlib
	if (isKeyPressed(KEY_NSPIRE_CTRL))	
#else
	if (keystate[SDLK_LCTRL])
#endif
	{
		ctrl_button = 1;
	}
	else
	{
		ctrl_button = 0;
	}
    
#ifdef ndlib
	if (isKeyPressed(KEY_NSPIRE_ESC))	
#else
	if (keystate[SDLK_ESCAPE])
#endif
	{
		esc = 1;
	}
	else
	{
		esc = 0;
	}
	
#ifdef ndlib
	if (isKeyPressed(KEY_NSPIRE_DEL))	
#else
	if (keystate[SDLK_BACKSPACE])
#endif
	{
		backspace_button = 1;
	}
	else
	{
		backspace_button = 0;
	}
	
#ifndef ndlib
    SDL_Event event;
    SDL_PollEvent(&event);
#endif
}

void set_fileid()
{
	/* fileid_selected is the id of the file selected (first dimension of the file_name array) */
	fileid_selected = choice + (scroll_choice*12);
}

/* 
 * Function to remove the file/folder:
 * First, it asks (via function delete_file_screen) if it wants to delete it
 * delete_file_screen() function returns the result
 * If it returns 1 ("YES") then the file/folder is deleted
 * If no, nothing happens
*/
void remove_file()
{
	char final[MAX_LENGH];
	int result;
	int isdeleted;
	
	result = -1;
	isdeleted = -1;
	
	last_folder = currentdir;
	
#ifdef NSPIRE
	if (file_type[fileid_selected] == F_C) 
	{
		sprintf(final, "%s%s/", currentdir, file_name[fileid_selected]);
	}
	else
	{
		sprintf(final, "%s%s", currentdir, file_name[fileid_selected]);
	}
#else
		sprintf(final, "%s/%s", currentdir, file_name[fileid_selected]);
#endif

	printf("%d\n",file_type[fileid_selected]);
	
	result = delete_file_screen();
	
	if (result == 1)
	{	
		if (file_type[fileid_selected] == F_C) 
		{
			isdeleted = clear_dir(final);
		}
		else
		{
			isdeleted = unlink(final);
		}
	}
	
	printf("%s\n",last_folder);
	
	list_all_files(last_folder);
	
	if (isdeleted == 0)
	{
		refresh_cursor(4);
	}
	else
	{
		refresh_cursor(3);
	}
	
}

unsigned char delete_file_screen()
{
	unsigned char choose_request;
	unsigned char done_request;
	choose_request = 0;
	done_request = 1;
	
	clear_entirescreen();
		
	if (file_type[fileid_selected] == F_C) 
	{
		print_string("Do you want to delete this folder ?",RED_C,0,0,60);	
	}
	else
	{
		print_string("Do you want to delete this file ?",RED_C,0,0,0);
	}
	
	print_string(file_name[fileid_selected],F_C,0,8,26);
	print_string("NO",F_C,0,32,80);
	print_string("YES",BLUE_C,0,32,100);
	
	CTRL_state = 2;
	CTRL_time = 0;
	
	update_entirescreen();
	
	while(done_request==1)
	{
		controls();
		
		if (UP_state > 0)
		{
			clear_entirescreen();
			print_string(file_name[fileid_selected],F_C,0,8,26);
			print_string("NO",F_C,0,32,80);
			print_string("YES",BLUE_C,0,32,100);
			update_entirescreen();
			choose_request = 0;
		}
		else if (DOWN_state > 0)
		{
			clear_entirescreen();
			print_string(file_name[fileid_selected],F_C,0,8,26);
			print_string("NO",BLUE_C,0,32,80);
			print_string("YES",F_C,0,32,100);
			update_entirescreen();
			choose_request = 1;
		}
		
		if (CTRL_state == 1)
		{
			done_request = 0;
		}
		
		/* Don't waste CPU cycles */
		#ifndef ndlib
			SDL_Delay(2);
		#endif
	}
	
	return choose_request;
}

void goto_folder()
{
	#ifndef NSPIRE
		char buf[1024];
	#endif
	
#ifdef NSPIRE
	sprintf(currentdir, "%s%s", currentdir, file_name[fileid_selected]);
#else
	sprintf(currentdir, "%s/%s", currentdir, file_name[fileid_selected]);
#endif
	
	#ifdef NSPIRE
		/* Set the current directory (with lots of "..") to the chosen one */
		NU_Set_Current_Dir(currentdir);
		/* This will return the current directory properly */
		NU_Current_Dir("A:",currentdir);
	#else
		/* Set it to the current directory. */
		chdir(currentdir);
		/* Get current directory. */
		currentdir = getcwd(buf, MAX_LENGH);
	#endif
	
	list_all_files(currentdir);
	refresh_cursor(2);
}

/*
	Draw the list of files on-screen (it is divided by the scroll_choice variable)
	For example, scroll_choice = 1 means that files from id 12 to 23 will be shown etc...
*/
void draw_files_list()
{
	unsigned char i;
	
	for (i = 0; i < 12; i++)
	{
		print_string(file_name[i+(scroll_choice*12)],file_type[i+(scroll_choice*12)],0,48,40+(16*i));
	}
}

void refresh_cursor(unsigned char all)
{	
	if (all == 2)
	{
		choice = 0;
		scroll_choice = 0;
		set_fileid();
		clear_entirescreen();
	}
	else if (all == 3 || all == 4)
	{
		clear_entirescreen();
	}
	
	/* Update position of cursor so we can limit the area to refresh*/
	gui_dst.y = 40+(16*choice) - 16;
	
#ifdef ndlib
	fillRect(gui_dst.x, gui_dst.y, gui_dst.w, gui_dst.h, 0);
#else
	SDL_FillRect(gui_screen, &gui_dst, 0);
#endif
	
	/* Then draw the cursor again after being cleared, it will be shown after we passed SDL_Flip/UpdateRect to it*/
	print_string("=>",RED_C,0,16,40+(choice*16) );
	
	draw_files_list();
	
	if (all == 1 || all == 2 || all == 3)
	{
		print_string(currentdir,GREEN_C,0,8,16);
		print_string("Select NES ROM",1200,0,8,6 );
		update_entirescreen();
	}
	else if (all == 4 )
	{
		print_string(currentdir,GREEN_C,0,8,16);
		print_string("File deleted !",1200,0,8,6 );
		update_entirescreen();
	}
	else
	{
#ifdef ndlib
		updateScreen();
#else	
		SDL_UpdateRect(gui_screen, gui_dst.x, gui_dst.y, gui_dst.w, gui_dst.h);
#endif
	}
}

void clear_entirescreen()
{
	#ifdef ndlib
		clearBufferB();
	#else
		SDL_FillRect(gui_screen, &gui_dst2, 0);
	#endif
}

void update_entirescreen()
{
	#ifdef ndlib
		updateScreen();
	#else	
		SDL_Flip(gui_screen);
	#endif
}


void list_all_files(char* directory)
{
	DIR *dir;
	struct dirent *ent;
	
	unsigned char temp;
	
	#if MAX_LENGH < 255
		unsigned char i;
	#elif MAX_LENGH < 65000
		unsigned short i;
	#else
		unsigned long i;
	#endif

	short pc;
	
	/* Reset all the stored files to zero */
	for(i=0;i<MAX_LENGH;i++)
	{
		strcpy(file_name[i], "");
	}
	
	i = 0;
	temp = 0;
	pc = -1;
	numb_files = 0;
	
	if ((dir = opendir (directory)) != NULL) 
	{
		while ( (ent = readdir (dir)) != NULL ) 
		{
			/* Add the .. string and then after, reject them all*/
			if (i == 0)
			{
				strcpy(file_name[i], "..");
				file_type[i] = TUR_C;
				i++;
				numb_files++;
			}
			
			/* Finds .tns occurence */
#ifdef NSPIRE
			char* pch = strstr (ent->d_name,".tns");
#else
			char* pch = strstr (ent->d_name,".nes");
#endif
			
			/* Reject these two signs and the executable itself */
			char* pch2 = strstr (ent->d_name,"..");
			char* pch3 = strstr (ent->d_name,"filebrw.tns");
			pc = strncmp (ent->d_name, ".", 2);
			
			/* Check if file in question is a folder */
			temp = is_folder(ent->d_name);
			
			/* If file has ".tns" extension, is a folder and is not ".." and "." */
			if ((pch2 == NULL && pch3 == NULL && pc != 0))
			{
				/* Copy string cotent from ent->d_name to file_name[i]*/
				strcpy(file_name[i], ent->d_name);
				
				if (pch != NULL)
				{
					file_type[i] = BLUE_C;
				}
				else if (temp)
				{
					file_type[i] = F_C;
				}
				else
				{
					file_type[i] = GREEN_C;
				}

				i++;
				numb_files++;
			}
			
		}
		closedir (dir);
	} 
	else 
	{
		perror ("Not a directory");
	}
	
	numb_files = numb_files - 1;
}

/* Draw a char on-screen. Thanks Exphophase for the code ! (coming form Gpsp, GPLv2) */
void screen_showchar(int x, int y, unsigned char a, int fg_color, int bg_color) 
{
#ifdef ndlib
	drawChar(&x, &y, 0, a, fg_color, bg_color);
#else
	unsigned short *dst;
	int w, h;

	SDL_LockSurface(gui_screen);
	
	for(h = 8; h; h--) 
	{
		dst = (unsigned short *)gui_screen->pixels + (y+8-h)*gui_screen->w + x;
		for(w = 8; w; w--) 
		{
			unsigned short color = *dst; // background
			if((fontdata8x8[a*8 + (8-h)] >> w) & 1) color = fg_color;
			*dst++ = color;
		}
	}
	
	SDL_UnlockSurface(gui_screen);
#endif
}

/* Draw a string on-screen. Thanks Exphophase for the code ! (coming form Gpsp, GPLv2) */
void print_string(char *s, unsigned short fg_color, unsigned short bg_color, int x, int y) 
{
#ifdef ndlib
	drawString(&x, &y, 0, s, fg_color, bg_color);
#else
	int i;
	int j;
	j = strlen(s);
	
	for(i = 0; i < j; i++, x += 6) 
	{
		screen_showchar(x, y, s[i], fg_color, bg_color);
	}
#endif
}

/* Is the path a folder ? */
unsigned char is_folder(char* str1)
{
	struct stat st;
	unsigned char temp;
	
	if( stat( str1, &st ) == 0 && S_ISDIR( st.st_mode ) ) 
	{
		temp = 1;
	} 
	else 
	{
		temp = 0;
	}
	 
	return temp;
}


unsigned char clear_dir(char* which_dir)
{
  DIR  *d;
  struct dirent *dir;
  char file[MAX_LENGH];
  
  d = opendir(which_dir);
  
  if (d)
  {
     while ((dir = readdir(d)) != NULL)
     {
        // exclude directories
        if( strcmp( dir->d_name, "." ) == 0 || strcmp( dir->d_name, ".." ) == 0)
        {
           continue;
        }

        sprintf(file,"%s/%s", which_dir, dir->d_name);

        if (opendir(file)!=NULL) //if file actually is a dir
        {
           clear_dir(file);
        }
        else
        {
           if (remove(file) == -1)
           {
              printf("\n%s\n", file);
              perror("Remove failed");
              return 1;
           }
        }
     }

     closedir(d);

     // Deleting directory
     if (rmdir(which_dir) == -1)
     {
        printf("%s\n", which_dir);
        perror("Remove failed");
        return 1;
     }

   }
  
   return(0);
}
