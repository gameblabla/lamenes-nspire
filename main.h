#define MAX_LENGH 254
#define EXECUTE_APP

/*	Colors	*/
#define BLUE_C  255
#define RED_C  1700
#define GREEN_C 512
#define TUR_C 750
#define F_C  1023

void init (void);
void controls ();

void refresh_cursor(unsigned char all);
void draw_files_list();

void screen_showchar(int x, int y, unsigned char a, int fg_color, int bg_color);
void print_string(char *s, unsigned short fg_color, unsigned short bg_color, int x, int y);

unsigned char is_folder(char* str1);
void list_all_files(char* directory);

void goto_folder();
void remove_file();
unsigned char clear_dir(char* which_dir);

void set_fileid();

#ifdef EXECUTE_APP
	void lamenes(char* rom_name);
#endif
