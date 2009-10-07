/* vim: set ts=4 sw=4 nowrap: */
/*
Copyright (C) 2009 Gary Stidston-Broadbent (gary@stroppytux.net)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/* Include our header files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <getopt.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>
#include "xlayout.h"
#include <ease.h>

/* Define program information */
const char *program				= "xlayout";
const char *author				= "Gary Stidston-Broadbent";
const char *contact				= "gary@stroppytux.net";
const char *version				= "0.8";
short int verbosity				= 5;

/* Define our global objects */
XLDisplay d;
XLPointer p;

/* Execute out main code block */
int main(int argc, char **argv)
{
	/* Define defaults before we start */
	d.name						= NULL;
	d.screen					= 0;
	d.screen_count				= 0;

	/* Local variables and flags */
	short int i;
	int option_index			= 0;
	char *geometry				= NULL;
	char *id					= NULL;
	char *name					= NULL;

	/* Option flags */
	int help_flag				= 0;
	int version_flag			= 0;
	int pointer_flag			= 0;
	int info_flag				= 0;
	int set_flag				= 0;
	int list_flag				= 0;
	int hide_flag				= 0;
	int root_flag				= 0;
	int show_flag				= 0;
	int top_flag				= 0;
#ifdef _HAVE_LIBEASE_
	int ease_flag				= 0;
#endif

	/* Set the options we recognise */
	struct option long_options[] = {
		/* Utility actions */
		{"verbose",		optional_argument,	0,				'v'},
		{"help",		no_argument,		0,				'h'},
		{"version",		no_argument,		&version_flag,	1},

		/* Display and screen options */
		{"display",		required_argument,	0,				'd'},
		{"screen",		required_argument,	0,				's'},

		/* Window and pointer information */
		{"pointer",		no_argument,		0,				'p'},
		{"fullscreen",	no_argument,		0,				'f'},
		{"top",			no_argument,		0,				't'},
		{"hide",		no_argument,		0,				'x'},
		{"root",		no_argument,		&root_flag,		1},
		{"geometry",	required_argument,	0,				'g'},
		{"id",			required_argument,	0,				'w'},
		{"name",		required_argument,	0,				'n'},
#ifdef _HAVE_LIBEASE_
		{"ease",		no_argument,		0,				'e'},
#endif
		/* Actions */
		{"set",			no_argument,		0,				'y'},
		{"info",		no_argument,		0,				'i'},
		{"show",		no_argument,		0,				'u'},
		{"list",		no_argument,		0,				'l'},
		{0, 0, 0, 0}
	};

	/* Process commandline options */
	while (1) {
		/* Get the option variable for this pass */
		i = getopt_long(argc,argv,"d:s:g:itn:hw:b:vufezpyxl",long_options, &option_index);
		if (i == -1) {break;}

		/* Run actions on our commands passed into the program */
		switch(i) {
			case 0:
				/* If this option set a flag, do nothing else now */
				if (long_options[option_index].flag != 0) {
					break;
				}
				debug(5, "option %s", long_options[option_index].name);
				if (optarg)
					debug(5, " with arg %s", optarg);
				debug(5, "\n");
				break;
			/* Set the verbosity */
			case 'v':
				if (optarg) {
					/* If no argument was set, increase the verbosity by one */
					verbosity = atoi(optarg);
				} else {
					verbosity++;
				}
				debug(8, "verbosity set to %d\n", verbosity);
				break;

			/* Display help information */
			case 'h':
				help_flag = 1;
				break;
			/* Display information */
			case 'i':
				info_flag = 1;
				break;
			/* Set information */
			case 'y':
				set_flag = 1;
				break;
			/* Hide an item */
			case 'x':
				hide_flag = 1;
				break;
			/* Set an item ontop */
			case 't':
				top_flag = 1;
				break;
			/* Show an item */
			case 'u':
				show_flag = 1;
				break;
			/* List windows */
			case 'l':
				list_flag = 1;
				break;
			/* Set the display */
			case 'd':
				debug(8, "Setting display to %s\n", optarg);
				d.name = optarg;
				break;
			/* Toggle actions on pointer instead of window */
			case 'p':
				debug(8, "Running actions on pointer\n");
				pointer_flag = 1;
				break;
				/* Set the screen */
			case 's':
				debug(8, "Setting screen to %s\n", optarg);
				d.screen = atoi(optarg);
				break;
			/* Set the geometry */
			case 'g':
				debug(8, "Setting geometry to %s\n", optarg);
				geometry = optarg;
				break;
			/* Set the geometry to fullscreen */
			case 'f':
				debug(8, "Setting geometry to fullscreen\n", optarg);
				geometry = "fullscreen";
				break;
			/* Set the window id */
			case 'w':
				debug(8, "Setting window id to %s\n", optarg);
				id = optarg;
				break;
			/* Set the window name */
			case 'n':
				debug(8, "Setting window name to %s\n", optarg);
				name = optarg;
				break;
#ifdef _HAVE_LIBEASE_
			/* Enable easing */
			case 'e':
				ease_flag = 1;
				break;
#endif
			case '?':
				break;
			default:
				abort();
		}
	}

	/* Execute our utility actions */
	(help_flag) ? display_help() : (void)NULL;
	(version_flag) ? display_version() : (void)NULL;

	/* Create a connection to the display */
	create_display();

	/* Create a connection to our window if need be */
	XLWindow w;
	w.id = NULL;
	w.name = NULL;
	if (root_flag || id || name) {
		/* Choose the window to use */
		if (root_flag) {
			debug(8, "Using window id '0'\n");
			w.id = "0";
		} else if (id) {
			debug(8, "Using window id %s\n", id);
			w.id = id;
		} else if (name) {
			debug(8, "Using window name %s\n", name);
			w.id = "n";
			w.name = name;
		}
	}

	/* Display info about the pointer */
	if (pointer_flag && info_flag) {
		display_pointer();
	}

	/* Set the location of the pointer */
	else if (pointer_flag && set_flag) {
		/* First check that the geometry is set */
		if (!geometry) {
			display_help();
			exit(1);
		} else {
			p.new_geometry = geometry;
#ifdef _HAVE_LIBEASE_
			if (ease_flag) {
				/* Get the current pointer position */
				XLPointer gp;
				get_pointer(&gp);
				debug(8, "Current pointer position is x:%d, y:%d\n", gp.x, gp.y);

				/* Get the geometry as x, y coordinates */
				int x, y, w, h, g;
				XSizeHints hints = {0};
				if (!XWMGeometry(d.display, d.screen, geometry, "100x100+100+100", 0, &hints, &x, &y, &w, &h, &g)) {
					debug(5, "Error getting new geometry (%s)\n", geometry);
					exit(1);
				}

				/* Create and populate our easing structure */
				Ease_Multi e;
				register int t;
				for (t=0; t<MULTI_MAX; ++t) e.dimension[t].type = '\0';

				/* Set the callback method */
				e.fpoint					= ease_pointer;

				/* Set the duration for both x and y to the largest value */
				int dx						= ((gp.x-x) < 0) ? ((gp.x-x)*-1) : gp.x-x;
				int dy						= ((gp.y-y) < 0) ? ((gp.y-y)*-1) : gp.y-y;
				int du						= (dx > dy) ? dx : dy;

				/* X dimension */
				e.dimension[0].type			= &easeOut;
				e.dimension[0].initial		= gp.x;
				e.dimension[0].duration		= du;
				e.dimension[0].difference	= x-gp.x;

				/* Y dimension */
				e.dimension[1].type			= &easeIn;
				e.dimension[1].initial		= gp.y;
				e.dimension[1].duration		= du;
				e.dimension[1].difference	= y-gp.y;

				/* Run our ease */
				ease_multi(&e);
			} else {
				set_pointer(geometry);
			}
#else
			set_pointer(geometry);
#endif
		}
	}

	/* Hide the pointer */
	else if (pointer_flag && hide_flag) {
		/* Hide the pointer */
		hide_pointer();
	}

	/* List the windows */
	else if (list_flag) {
		list_windows();
	}

	/* Display information about a window */
	else if (!pointer_flag && info_flag) {
		/* Make sure a window was set */
		(!w.id && !w.name) ? display_help() : (void)NULL;

		/* Create the connection to the window, then display info */
		create_window(&w);
		display_window(&w);
	}

	/* Set the windows geometry */
	else if (!pointer_flag && set_flag) {
		/* Make sure a window was set */
		(!w.id && !w.name && !geometry) ? display_help() : (void)NULL;

		/* Create the connection to the window, then display info */
		w.new_geometry = geometry;
		create_window(&w);

#ifdef _HAVE_LIBEASE_
			if (ease_flag) {
				/* Get the current window position */
				XLWindow gw;
				gw=w;
				get_window(&gw);
				debug(8, "Current window dimensions are x:%d, y:%d, w:%d, h:%d\n", gw.x, gw.y, gw.w, gw.h);

				/* Get the geometry as x, y coordinates */
				int x, y, wi, hi, g;
				XSizeHints hints = {0};
				if (!XWMGeometry(d.display, d.screen, geometry, "100x100+100+100", 0, &hints, &x, &y, &wi, &hi, &g)) {
					debug(5, "Error getting new geometry (%s)\n", geometry);
					exit(1);
				}

				/* Create and populate our easing structure */
				Ease_Multi e;
				register int t;
				for (t=0; t<MULTI_MAX; ++t) e.dimension[t].type = '\0';

				/* Set the callback method */
				e.fpoint					= ease_window;

				/* Set the duration for both x and y to the largest value */
				int dx						= ((gw.x-x) < 0) ? ((gw.x-x)*-1) : gw.x-x;
				int dy						= ((gw.y-y) < 0) ? ((gw.y-y)*-1) : gw.y-y;
				int du						= (dx > dy) ? dx : dy;

				/* Set the duration for both w and h to the largest value */
				int dw						= ((gw.w-wi) < 0) ? ((gw.w-wi)*-1) : gw.w-wi;
				int dh						= ((gw.h-hi) < 0) ? ((gw.h-hi)*-1) : gw.h-hi;
				int ds						= (dw > dh) ? dw : dh;

				/* X dimension */
				e.dimension[0].type			= &easeOut;
				e.dimension[0].initial		= gw.x;
				e.dimension[0].duration		= du;
				e.dimension[0].difference	= x-gw.x;

				/* Y dimension */
				e.dimension[1].type			= &easeIn;
				e.dimension[1].initial		= gw.y;
				e.dimension[1].duration		= du;
				e.dimension[1].difference	= y-gw.y;

				/* W dimension */
				e.dimension[2].type			= &easeOut;
				e.dimension[2].initial		= gw.w;
				e.dimension[2].duration		= ds;
				e.dimension[2].difference	= wi-gw.w;

				/* H dimension */
				e.dimension[3].type			= &easeIn;
				e.dimension[3].initial		= gw.h;
				e.dimension[3].duration		= ds;
				e.dimension[3].difference	= hi-gw.h;

				debug(10, "du:%d, xi:%d, xd:%d\n", du, e.dimension[0].initial, e.dimension[0].difference);
				debug(10, "du:%d, yi:%d, yd:%d\n", du, e.dimension[1].initial, e.dimension[1].difference);
				debug(10, "ds:%d, wi:%d, wd:%d\n", ds, e.dimension[2].initial, e.dimension[2].difference);
				debug(10, "ds:%d, hi:%d, hd:%d\n", ds, e.dimension[3].initial, e.dimension[3].difference);

				/* Run our ease */
				ease_multi(&e, w);
			} else {
				set_window(&w);
			}
#else
		set_window(&w);
#endif
	}

	/* Hide the window */
	else if (!pointer_flag && hide_flag) {
		/* Make sure a window was set */
		(!w.id && !w.name) ? display_help() : (void)NULL;

		/* Create the connection to the window, then hide it */
		create_window(&w);
		hide_window(&w);
	}

	/* Show the window */
	else if (!pointer_flag && show_flag) {
		/* Make sure a window was set */
		(!w.id && !w.name) ? display_help() : (void)NULL;

		/* Create the connection to the window, then show it */
		create_window(&w);
		show_window(&w);
	}

	/* Set the window on top of all others */
	else if (!pointer_flag && top_flag) {
		/* Make sure a window was set */
		(!w.id && !w.name) ? display_help() : (void)NULL;

		/* Create the connection to the window, then set it ontop */
		create_window(&w);
		top_window(&w);
	}

	/* Close the connection to the display */
	destroy_display();
	exit(0);
}

/* Display information to the user if the information level is low enough */
void debug(short int level, char *msg, ...)
{
	/* Use va_list to get variadic options */
	va_list fmtargs;
	char buffer[1024];
	va_start(fmtargs,msg);
	vsnprintf(buffer,sizeof(buffer)-1,msg,fmtargs);
	va_end(fmtargs);

	/* Check the level is less than the verbosity level */
	if (level <= verbosity) {
		printf("%s",buffer);
	}
}

/* Display program version information */
void display_version()
{
	debug(5, "%s ", program);
	debug(1, "%s\n", version);
	debug(5, "Copyright (C) 2009 %s\n", author);
	debug(5, "License GPLv2: GNU GPL version 2 <http://gnu.org/licenses/gpl.html>\n");
	debug(5, "This is free software: you are free to change and redistribute it.\n");
	debug(5, "There is NO WARRANTY, to the extent permitted by law.\n");
	debug(5, "\nWritten by %s\n", author);
	exit(0);
}

/* Display program help information */
void display_help()
{
	debug(5, "usage:  %s [-options ...]\n\n", program);
	debug(5, "where options include:\n");
	debug(5, "    --version            Print the version\n");
	debug(5, "    --help               Print this message\n");
	debug(5, "    --list type          List either 'screens' or 'windows'\n");
	debug(5, "    --info               Display window information\n");
	debug(5, "    --set                Set window information\n");
	debug(5, "    --display host:dpy   X server to contact\n");
	debug(5, "    --root               Use the root window\n");
	debug(5, "    --id windowid        Use the window with the specified id\n");
	debug(5, "    --name windowname    Use the window with the specified name\n");
	debug(5, "    --pointer            Set/show the pointer instead of a window\n");
	debug(5, "    --geometry           Set the geometry of the id/name/pointer\n");
	debug(5, "    --fullscreen         Set window to fullscreen\n");
	debug(5, "    --top                Set window to display on the top of all others\n");
	debug(5, "    --hidden             Hide the window or pointer\n");
	debug(5, "    --show               Unhide the window or pointer\n");
	debug(5, "    --verbose            Increase the verbosity of the output\n");
	debug(5, "\n");
	debug(5, "examples:\n");
	debug(5, "    List windows:  xlayout --display :1.0 --list windows\n");
	debug(5, "    Show win info: xlayout --display :0.0 --show --id 0xa00001\n");
	debug(5, "                   xlayout --info --name \"www.google.co.uk - Mozilla Firefox\"\n");
	debug(5, "    Set window:    xlayout --set --name MPlayer --fullscreen\n");
	debug(5, "                   xlayout --name MPlayer --hidden\n");
	debug(5, "                   xlayout --display domain.com:0.0 --name MPlayer --top\n");
	debug(5, "    Show pointer:  xlayout --info --pointer --display :0.0\n");
	debug(5, "    Set pointer:   xlayout --set --pointer --geometry 0x0+100+100\n");
	debug(5, "\n");
	exit(0);
}

/****************************************************************************/
/* Display functions														*/
/****************************************************************************/
/* Create a connection to our display and screen */
int create_display()
{
	/* Try open a connection to the display */
	if ((d.display = XOpenDisplay(d.name)) == NULL) {
		debug(5, "Unable to open display '%s'\n", XDisplayName(d.name));
		exit(1);
	}

	/* Set the default screen to use "screen(0)" */
	debug(10, "Setting default screen for %s\n", d.name);
	d.screen = DefaultScreen(d.display);
	return (0);
}

/* Destroy the connection to the display */
int destroy_display()
{
	debug(10, "Destroying display %s\n", d.name);
	/* First flush the display to make sure our actions have been committed */
	XFlush(d.display);
	XCloseDisplay(d.display);
	return(0);
}

/* Return the quantity of screens attached to the X server */
int count_screens()
{
	d.screen_count	= XScreenCount(d.display);
	debug(5, "There are ");
	debug(1, "%d", d.screen_count);
	debug(5, "screens\n");
	return(0);
}

/****************************************************************************/
/* Pointer functions														*/
/****************************************************************************/
/* Display information about the pointer */
void display_pointer()
{
	/* Create a connection to the root window */
	XLWindow w;
	w.id = "0";
	create_window(&w);
	int x, y, dummy;
	unsigned int mask;

	/* Call XQueryPointer to get the resulting x and y coordinates */
	debug(10, "Getting pointer coordinates\n", d.name);
	XQueryPointer(d.display, w.window, &w.window, &w.window, &x, &y, &dummy, &dummy, &mask);
	debug(5, "X Position: ");
	debug(1, "%d\n", x);
	debug(5, "Y Position: ");
	debug(1, "%d\n", y);
}

/* Get the pointer information */
void get_pointer(XLPointer *gp)
{
	/* Create a connection to the root window */
	XLWindow w;
	w.id = "0";
	create_window(&w);
	int x, y, dummy;
	unsigned int mask;

	/* Call XQueryPointer to get the resulting x and y coordinates */
	debug(10, "Getting pointer coordinates\n");
	XQueryPointer(d.display, w.window, &w.window, &w.window, &x, &y, &dummy, &dummy, &mask);
	gp->x = x;
	gp->y = y;
}

/* Set the location of the pointer */
void set_pointer(char *geometry)
{
	/* Create a connection to the root window */
	XLWindow root;
	root.window = 0;
	root.id = "0";
	create_window(&root);
	int x, y, w, h, g;

	/* Set the pointer location */
	debug(10, "Setting pointer coordinates to ");
	debug(8, "%s\n", geometry);
	XSizeHints hints = {0};
	if (!XWMGeometry(d.display, d.screen, geometry, "100x100+100+100", 0, &hints, &x, &y, &w, &h, &g)) {
		debug(5, "Error setting WMGeometry\n");
		exit(1);
	}

	/* Now move the pointer into place */
	if (!XWarpPointer(d.display, root.window, root.window, 0, 0, 0, 0, x, y)) {
		debug(5, "Error setting pointer\n");
		exit(1);
	}
}

#ifdef _HAVE_LIBEASE_
/* Callback to set the pointer for easing */
int ease_pointer(Ease_Multi *e, va_list ap)
{
	char geometry[20];
	sprintf(geometry, "0x0+%d+%d", e->dimension[0].value, e->dimension[1].value);
	/* Set the pointer to the new geometry */
	usleep(10000);
	set_pointer(geometry);
	XFlush(d.display);
	return 0;
}
#endif

/* Hide the pointer */
void hide_pointer()
{
	/* Creating a new single pixel image for the pointer */
	debug(5, "Setting the pointer to hidden\n");
	p.pix = XCreatePixmap(d.display, DefaultRootWindow(d.display), 1, 1, 1);
	p.colour.pixel = 0;
	p.colour.red = 0;
	p.colour.green = 0;
	p.colour.blue = 0;
	p.colour.flags = 0;
	p.colour.pad = 0;

	/* Assign the pixel to the cursor */
	p.blank = XCreatePixmapCursor(d.display, p.pix, p.pix, &p.colour, &p.colour, 1, 1);
	XDefineCursor(d.display, DefaultRootWindow(d.display), p.blank);
	XFlush(d.display);
	return;
}

/****************************************************************************/
/* Window functions															*/
/****************************************************************************/
/* Create a connection to the window */
void create_window(XLWindow *w)
{
	/* Check if we are trying to connect to the root window */
	if (!strcmp(w->id, "0")) {
		debug(8, "Creating connection to root window (%s)\n", w->id);
		w->window = 0;
		w->window = RootWindow(d.display,d.screen);
		return;
	}

	/* Set the window using an id */
	if (w->id && strcmp(w->id, "n")) {
		debug(8, "Creating connection to window %s\n", w->id);
		w->window = 0;

		/* First try with a hex format */
		debug(10, "Trying %s with hex format\n", w->id);
		sscanf(w->id, "0x%lx", &w->window);

		/* If that didnt work try with a decimal format */
		if (!w->window) {
			debug(10, "Trying %s with decimal format\n", w->id);
			sscanf(w->id, "%ld", &w->window);
		}
		if (!w->window) {
			debug(5, "Invalid window id format: %s.\n", w->id);
			exit(1);
		} else {
			debug(10, "Window id (%s) set, now connecting\n", w->id);
			w->window = XmuClientWindow(d.display, w->window);
			return;
		}
	}

	/* Set the window using the window name */
	if (strcmp(w->name, "NULL")) {
		debug(8, "Trying to connect to window named %s\n", w->name);
		Window root;
		char *name;
		char *tmp_id;
		name = w->name;
		root = RootWindow(d.display,d.screen);
		w->window = create_window_named(name, root);
		sprintf(tmp_id, "0x%lx", w->window);
		w->id = tmp_id;
	}

	/* We cant set the window, return an error */
	if (!w->window) {
		debug(5, "There was an error selecting the window!\n");
		exit(1);
	}
	return;
}

/* Select a window by the window name */
Window create_window_named(char *winsName, Window root)
{
	debug(8, "Connection to window %s\n", winsName);
	Window *children, dummy;
	unsigned int child_count;
	char *window_name;
	if (XFetchName(d.display, root, &window_name) && strcmp(window_name, winsName) == 0) {
	  return(root);
	}
	if (!XQueryTree(d.display, root, &dummy, &dummy, &children, &child_count)) {
		return(0);
	}

	while (child_count--) {
		root = create_window_named(winsName,children[child_count]);
		if (root)
			break;
	}

	if (children) XFree ((char *)children);
	return(root);
}

/* Map the window for view */
void show_window(XLWindow *w)
{
	XMapWindow(d.display, w->window);
	XRaiseWindow(d.display, w->window);
	return;
}

/* Hide the window from view */
void hide_window(XLWindow *w)
{
	XUnmapWindow(d.display,w->window);
	return;
}

/* Set window ontop of all others */
void top_window(XLWindow *w)
{
	XMapWindow(d.display, w->window);
	XRaiseWindow(d.display, w->window);
	return;
}

/* Set the window border */
void window_border(XLWindow *w)
{
	XSetWindowBorderWidth(d.display, w->window, w->new_border);
	w->border = w->new_border;
	return;
}

/* Get the window information */
void get_window(XLWindow *gw)
{
	XWindowAttributes winAttribs;
	if (XGetWindowAttributes(d.display, gw->window, &winAttribs) == 0) {
		debug(5, "failed to get window attributes\n");
		exit(1);
	}

	/* Set our windows attributes to those returned */
	gw->x		 		= winAttribs.x;
	gw->y		 		= winAttribs.y;
	gw->w		 		= winAttribs.width;
	gw->h		 		= winAttribs.height;
	gw->border			= winAttribs.border_width;
}

/* Set the window position */
void set_window(XLWindow *w)
{
	debug(8, "Setting window to %s\n", w->new_geometry);
	int rx, ry, rw, rh, rg;
	XSizeHints hints = {0};
	if (strcmp(w->new_geometry, "fullscreen")) {
		XWMGeometry(d.display, d.screen, w->new_geometry, "200x200+0+0", 0, &hints, &rx, &ry, &rw, &rh, &rg);
	} else {
		XWindowAttributes rootWinAttribs;
		if (XGetWindowAttributes(d.display, RootWindow(d.display,d.screen), &rootWinAttribs) == 0) {
			printf("Failed to get window attributes\n");
			exit(1);
		}
		rx 	= rootWinAttribs.x;
		ry 	= rootWinAttribs.y;
		rw 	= rootWinAttribs.width;
		rh 	= rootWinAttribs.height;
	}

	w->x = rx;
	w->y = ry;
	w->w = rw;
	w->h = rh;
	XMoveResizeWindow(d.display, w->window, w->x, w->y, w->w, w->h);
	return;
}

#ifdef _HAVE_LIBEASE_
/* Callback to set the window for easing */
int ease_window(Ease_Multi *e, va_list ap)
{
	char geometry[20];
	XLWindow w;
	w = va_arg(ap, XLWindow);

	sprintf(geometry, "%dx%d+%d+%d", e->dimension[2].value, e->dimension[3].value, e->dimension[0].value, e->dimension[1].value);
	usleep(1000);

	/* Set the window to the new geometry */
	w.new_geometry = geometry;
	set_window(&w);
	XFlush(d.display);
	return 0;
}
#endif

/* Return a list of the windows on the X11 server */
void list_windows()
{
	Window *children, dummy, root;
	root = RootWindow(d.display,d.screen);
	unsigned int child_count;
	XQueryTree(d.display, root, &dummy, &dummy, &children, &child_count);
	debug(5, "ID			NAME\n\n");

	char *tmp_name;
	while (child_count--) {
		tmp_name = NULL;
		XFetchName(d.display, children[child_count], &tmp_name);
		debug(5, "0x%lx",children[child_count]);
		tmp_name ? debug(5, "\t\t%s\n",tmp_name) : debug(5, "\t\tNA\n");
	}
}

/* Show information about a window */
void display_window(XLWindow *w)
{
	/* Fetch the windows name */
	debug(8, "Fetching windows name for %s\n", w->id);
	char *tmp_name;
	XFetchName(d.display, w->window, &tmp_name);
	w->name = (tmp_name) ? tmp_name : "Root";
	debug(8, "Windows name returned %s\n", w->name);

	/* Fetch the windows attributes */
	debug(8, "Fetching windows attributes for %s\n", w->id);
	XWindowAttributes winAttribs;
	if (XGetWindowAttributes(d.display,w->window,&winAttribs) == 0) {
		debug(5, "failed to get window attributes\n");
		exit(1);
	}

	/* Set our windows attributes to those returned */
	w->x		 		= winAttribs.x;
	w->y		 		= winAttribs.y;
	w->w		 		= winAttribs.width;
	w->h		 		= winAttribs.height;
	w->border			= winAttribs.border_width;

	/* Display the info to the user */
	debug(5, "Window Name:   %s\n",w->name);
	debug(5, "Window Id:     %s\n", w->id);
	debug(5, "geometry:      ");
	debug(1, "%dx%d+%d+%d\n", w->w, w->h, w->x, w->y);
	debug(5, "X Position:    %d\n",w->x);
	debug(5, "Y Position:    %d\n",w->y);
	debug(5, "Window Width:  %d\n",w->w);
	debug(5, "Window Height: %d\n",w->h);
	debug(5, "Border Width:  %d\n",w->border);
	return;
}
