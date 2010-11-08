// TODO: Add proper comments where needed!
#include "data.h"
// Globals
Window main_win;
Display *dpy;
XWindowAttributes wa;

button *bt_arr[BUTTON_AMOUNT];

int main(int argc, char* argv[]){
	char bg[16] = "White";		// Window background
	// Handle args as needed.
	if(argc == 2 && !strcmp("-v", argv[1])){
		fprintf(stdout, "XPaint versoin %s\n", VERSION);
		return 0;
	}
	if(argc == 3 && !strcmp("-bg", argv[1])){
		if(strlen(argv[2]) <= 16){
			fprintf(stdout, "Background Color: %s\n", argv[2]);
			strcpy(bg, argv[2]);
		}else{
			fprintf(stderr, "BG Color too long");
			return 1;
		}
	}
	// Try Opening a connection to Xorg server and check if it worked
	if((dpy = XOpenDisplay(NULL)) == NULL){
		fprintf(stderr, "Couldn't open display\n Shutting down...\n");
		return 1;
	};
	// Draw the main window and send all the needed data from it (GC), to main loop
	//ev_loop(draw_main(bg));
	init_main_window(bg);
	//draw_main(bg);
	ev_loop();
	(void)XCloseDisplay(dpy);

	return 0;	
}

// Initial drawing of the main window
void init_main_window(char *bg){
	unsigned long int mask = ButtonPressMask | ButtonReleaseMask | ExposureMask | StructureNotifyMask;
	Window root = XDefaultRootWindow(dpy);
	// Generate main window
	main_win = XCreateSimpleWindow(dpy, root, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, LINE_SIZE, get_color("Pink"), get_color(bg));
	// Select what events to respond to
	XSelectInput(dpy, main_win, mask);
	// Show the main window on the screen
	XMapWindow(dpy, main_win);
}

GC init_gc(char *bg){
	XGCValues gcvals;
	unsigned long int vals = GCBackground | GCForeground;

	gcvals.background = get_color(bg);
	gcvals.foreground = get_color("Black");

	return XCreateGC(dpy, main_win, vals, &gcvals);
}

// Return the ID of a String Color Name
unsigned long get_color(const char* color_name){
	Colormap colormap = DefaultColormap(dpy, DefaultScreen(dpy));
	XColor c1, c2;		// Color1, Color2
	
	XAllocNamedColor(dpy, colormap, color_name, &c1, &c2);

	return c1.pixel;
}

// Return the GC with the colors we want, bg doesn't seem to be used for much.
GC change_color(const char* fg, const char* bg){
	XGCValues gcvals;
	unsigned long int vals = GCForeground | GCBackground | GCLineWidth;

	gcvals.foreground = get_color(fg);
	gcvals.background = get_color(bg);
	gcvals.line_width = LINE_SIZE;

	return XCreateGC(dpy, main_win, vals, &gcvals);
}

void draw_button(button *bt) {
	// Draw the buttons _outline_
	XDrawRectangle(dpy, main_win, change_color(FOREGROUND_COLOR, BACKGROUND_COLOR), bt->x, bt->y, BUTTON_WIDTH,BUTTON_HEIGHT);
	switch(bt->type){
	case COLOR:
	// Fill the outline with our chosen color
	XFillRectangle(dpy, main_win, change_color(bt->mode.color, "Black"), bt->x + 1, bt->y + 1, BUTTON_WIDTH - 1, BUTTON_HEIGHT - 1);
	break;
	case SHAPE:
	// Nested case? Blarghhh...
	if(bt->mode.shape_id == LINE){
		XDrawLine(dpy, main_win, change_color("Black", "White"), bt->x + 2, bt->y + 2, bt->x + BUTTON_WIDTH - 2, bt->y + BUTTON_HEIGHT - 2);
	}
	if(bt->mode.shape_id == RECTANGLE){
		XDrawRectangle(dpy, main_win, change_color("Black", "White"), bt->x + 2, bt->y + 2, BUTTON_WIDTH - 4, BUTTON_HEIGHT - 4);
	}
	break;
	case EXIT:
	break;
	case CHANGE_GC:
	break;
	}
}

int mouse_event(bt_shape_id *shape, int bp_x, int bp_y, GC *gc){
	int i;
	
	for(i = 0; i < BUTTON_AMOUNT; i++){
		if(bp_x >= bt_arr[i]->x && bp_y >= bt_arr[i]->y && bp_x <= bt_arr[i]->x + BUTTON_WIDTH && bp_y <= bt_arr[i]->y + BUTTON_HEIGHT){
			switch(bt_arr[i]->type){
			case COLOR:
				*gc = change_color(bt_arr[i]->mode.color, "Black");
				break;
			case SHAPE:
			*shape = bt_arr[i]->mode.shape_id;
			break;
			case CHANGE_GC:
			// TODO: Handle this, mostly LINE_SIZE, for now.
			break;
			case EXIT:
				return 1;	// Quit/Exit program
			break;
			}
		}
	}
	return 0;
}

int ev_loop(){
	GC gc = init_gc("Black");
	
	XEvent ev;
	int quit = 0;
	int i;

	int bp_x, bp_y;		// X&Y on mouse button press.
	int br_x, br_y;		// X&Y on mouse button release.
	int w, h;

	button bt_close, bt_line, bt_rectangle, bt_red, bt_blue;
	bt_shape_id shape = LINE;

	// TODO:
	// Make an add_button()
	bt_close.x = 3;
	bt_close.y = 3;
	strcpy(bt_close.mode.color, "Black");
	bt_close.type = EXIT;

	bt_red.x = 26;
	bt_red.y = 3;
	strcpy(bt_red.mode.color, "Red");
	bt_red.type = COLOR;

	bt_blue.x = 49;
	bt_blue.y = 3;
	strcpy(bt_blue.mode.color, "Blue");
	bt_blue.type = COLOR;

	bt_line.x = 72;
	bt_line.y = 3;
	bt_line.mode.shape_id = LINE;
	bt_line.type = SHAPE;

	bt_rectangle.x = 95;
	bt_rectangle.y = 3;
	bt_rectangle.mode.shape_id = RECTANGLE;
	bt_rectangle.type = SHAPE;

	bt_arr[0] = &bt_close;
	bt_arr[1] = &bt_red;
	bt_arr[2] = &bt_blue;
	bt_arr[3] = &bt_line;
	bt_arr[4] = &bt_rectangle;


	while(!quit){
		XNextEvent(dpy, &ev);

		switch(ev.type){
			case Expose:
			for(i = 0; i < BUTTON_AMOUNT; i++){
				draw_button(bt_arr[i]);
			}
			if(XGetWindowAttributes(dpy, main_win, &wa) == 0){
				fprintf(stderr,"Couldn't get Window Attributes\n");
				return 1;
			}
			break;
			case ConfigureNotify:
			// TODO: Handle resizing by WM
			// This is very wrong, it's a roundtrip call and slows the app real bad.
			// Get the X,Y from the event var.
			if(XGetWindowAttributes(dpy, main_win, &wa) == 0){
				fprintf(stderr,"Couldn't get Window Attributes\n");
				return 1;
			}
			break;
			case ButtonPress:
				bp_x = ev.xbutton.x;
				bp_y = ev.xbutton.y;
			break;
			case ButtonRelease:
				br_x = ev.xbutton.x;
				br_y = ev.xbutton.y;
				w = br_x - bp_x;
				h = br_y - bp_y;

				if(w < 0){ w = w * (-1);}
				if(h < 0){ h = h * (-1);}

			quit = mouse_event(&shape, bp_x, bp_y, &gc);
			if(quit == 0){
				switch(shape){
					case LINE:
					fprintf(stdout, "Before Line x:%d|y:%d\n", ev.xbutton.x, ev.xbutton.y);
					XDrawLine(dpy, main_win,gc, bp_x, bp_y, ev.xbutton.x, ev.xbutton.y);
					fprintf(stdout, "After Line x:%d|y:%d\n", ev.xbutton.x, ev.xbutton.y);
					break;
					case RECTANGLE:
					// Fucking horrible shit, Xlib doesn't handle negative rectangle drawing
					// If drown down and left
					if((bp_x < br_x) && (bp_y < br_y)){
					XDrawRectangle(dpy, main_win, gc, bp_x, bp_y, w, h);
					}
					// If drawing down and right
					if((bp_x > br_x) && (bp_y < br_y)){
						XDrawRectangle(dpy, main_win, gc, br_x, bp_y, w, h);
					}
					// If drawomg up and right
					if((bp_x > br_x) && (bp_y > br_y)){
						XDrawRectangle(dpy, main_win, gc, br_x, br_y, w, h);
					}
					// If drawing up and left
					if((bp_x < br_x) && (bp_y > br_y)){

						XDrawRectangle(dpy, main_win, gc, bp_x, br_y, w, h);
					}
					break;
				}
			}
			break;
			case DestroyNotify:
				quit = 1;
			break;
		}
	}
	return 0;
}


