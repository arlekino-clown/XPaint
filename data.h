//#include "X11_Paint.c"

// Standard
#ifndef STDIO_H
	#include <stdio.h>
	#define STDIO_H
#endif

#ifndef STDLIB_H
	#include <stdlib.h>
	#define STDLIB_H
#endif
#ifndef STRING_H
	#include <string.h>
	#define STRING_H
#endif

//X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

// Default/Testing values
#define VERSION "0.02"

#define WINDOW_WIDTH 50
#define WINDOW_HEIGHT 100
#define BACKGROUND_COLOR "White"
#define FOREGROUND_COLOR "Black"

#define BUTTON_AMOUNT 5
#define BUTTON_WIDTH 20
#define BUTTON_HEIGHT 20
#define BUTTON_DIST 3		// Distance of buttons lined in a row

#define LINE_SIZE 1
// Consider validity of using this struct.
typedef struct _display_info{
	Display *dpy;
	Window root;
	Window win_main;
	int screen;	
} display_info;

// What the button does
typedef enum _bt_type { COLOR, SHAPE, EXIT, CHANGE_GC } bt_type;

// What we're drawing
typedef enum _bt_shape_id { NONE, LINE, RECTANGLE, ELIPSE } bt_shape_id;

typedef union _bt_mode{
	bt_shape_id shape_id;
	char color[32];
}bt_mode;

typedef struct _button{
	int x, y;
	bt_type type;			// What type of function
	bt_mode mode;			// What to do for that function
}button;

// Function Prototypes

unsigned long int get_color(const char* color_name);
GC change_color(const char* fg, const char* bg);

GC draw_main(char *bg);
void init_main_window(char *bg);
int ev_loop();

void draw_button(button *bt);
int mouse_even(int x, int y, GC *gc);

