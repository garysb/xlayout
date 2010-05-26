/* vim: set ts=4 sw=4 nowrap: */
/*
Copyright (C) 2010 Gary Stidston-Broadbent (xlayout@stroppytux.net)

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

/* Set program constants */
const char *author				= "Gary Stidston-Broadbent";
short int verbosity				= 5;

/* Define our global objects */
XLDisplay d;
XLPointer p;

/* If we have regex, define a global regex compile */
#ifdef HAVE_REGEX_H
regex_t re;
#endif

/* Execute our main code block */
int main(int argc, char **argv)
{
	/* Set defaults before we start */
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
#ifdef HAVE_EASE_MULTI
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
		{"screen",		required_argument,	0,				'c'},

		/* Pointer information */
		{"pointer",		no_argument,		0,				'p'},

		/* Window information */
		{"fullscreen",	no_argument,		0,				'f'},
		{"root",		no_argument,		&root_flag,		1},
		{"geometry",	required_argument,	0,				'g'},
		{"id",			required_argument,	0,				'w'},
		{"name",		required_argument,	0,				'n'},

		/* Actions */
		{"list",		no_argument,		0,				'l'},
		{"info",		no_argument,		0,				'i'},
		{"set",			no_argument,		0,				's'},
#ifdef HAVE_EASE_MULTI
		{"ease",		no_argument,		0,				'e'},
#endif
		{"show",		no_argument,		0,				'u'},
		{"top",			no_argument,		0,				't'},
		{"hide",		no_argument,		0,				'x'},
		{0, 0, 0, 0}
	};

	/* Process commandline options */
	while (1) {
		/* Get the option variable for this pass */
		i = getopt_long(argc,argv,"vhbd:c:pftxg:w:n:esiul",long_options, &option_index);
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
				debug(10, "verbosity set to %d\n", verbosity);
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
			case 's':
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
				debug(9, "Setting display to %s\n", optarg);
				d.name = optarg;
				break;
			/* Toggle actions on pointer instead of window */
			case 'p':
				debug(9, "Running actions on pointer\n");
				pointer_flag = 1;
				break;
				/* Set the screen */
			case 'c':
				debug(9, "Setting screen to %s\n", optarg);
				d.screen = atoi(optarg);
				break;
			/* Set the geometry */
			case 'g':
				debug(9, "Setting geometry to %s\n", optarg);
				geometry = optarg;
				break;
			/* Set the geometry to fullscreen */
			case 'f':
				debug(9, "Setting geometry to fullscreen\n", optarg);
				geometry = "fullscreen";
				break;
			/* Set the window id */
			case 'w':
				debug(9, "Setting window id to %s\n", optarg);
				id = optarg;
				break;
			/* Set the window name */
			case 'n':
				debug(9, "Setting window name to %s\n", optarg);
				name = optarg;
#ifdef HAVE_REGEX_H
				/* Compile our regex expression */
				if (regcomp(&re, name, REG_EXTENDED|REG_NOSUB) != 0) {
					debug(5, "Invalid regular expression \"%s\"\n", name);
					exit(1);
				}
#endif
				break;
#ifdef HAVE_EASE_MULTI
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
	(help_flag||argc==1) ? display_help() : (void)NULL;
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
			debug(9, "Using window id '0' (root window)\n");
			w.id = "0";
		} else if (id) {
			debug(9, "Using window id %s\n", id);
			w.id = id;
		} else if (name) {
			debug(9, "Using window named %s\n", name);
			w.id = "n";
			w.name = name;
		}
	}

	/* Display info about the pointer */
	if (pointer_flag && (info_flag || (!set_flag && !hide_flag && !show_flag && !geometry))) {
		display_pointer();
	}

	/* Set the location of the pointer */
	else if (pointer_flag && ((set_flag || geometry) && !show_flag && !hide_flag)) {
		/* First check that the geometry is set */
		if (!geometry) {
			display_help();
			exit(1);
		} else {
			p.new_geometry = geometry;
#ifdef HAVE_EASE_MULTI
			if (ease_flag) {
				/* Get the current pointer position */
				XLPointer gp;
				get_pointer(&gp);
				debug(9, "Current pointer position is x:%d, y:%d\n", gp.x, gp.y);

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
				int dx						= ((gp.x-x) < 0) ? ((gp.x-x)*-1) : gp.x-x;;
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

	/* Show the pointer */
	else if (pointer_flag && show_flag) {
		/* Unhide the pointer */
		show_pointer();
	}

	/* List the windows */
	else if (list_flag) {
		Window root;
		root = RootWindow(d.display,d.screen);
		list_windows(root, 0);
	}

	/* Display information about a window */
	else if (!pointer_flag && (info_flag || (!set_flag && !top_flag && !show_flag && !hide_flag && !geometry))) {
		/* Make sure a window was set */
		(!w.id && !w.name) ? display_help() : (void)NULL;

		/* Create the connection to the window, then display info */
		create_window(&w);
		display_window(&w);
	}

	/* Set the windows geometry */
	else if (!pointer_flag && ((set_flag || geometry) && !top_flag && !show_flag && !hide_flag)) {
		/* Make sure a window was set */
		(!w.id && !w.name && !geometry) ? display_help() : (void)NULL;

		/* Create the connection to the window, then display info */
		w.new_geometry = geometry;
		create_window(&w);

#ifdef HAVE_EASE_MULTI
			if (ease_flag) {
				/* Get the current window position */
				XLWindow gw;
				gw=w;
				get_window(&gw);

				/* Get the given geometry as x, y coordinates */
				int x, y, wi, hi, g;
				XSizeHints hints = {0};
				if (!XWMGeometry(d.display, d.screen, geometry, "100x100+100+100", 0, &hints, &x, &y, &wi, &hi, &g)) {
					/* We couldnt get the given geometry, report an error an exit */
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
	debug(5, "%s ", PACKAGE);
	debug(1, "%s\n", PACKAGE_VERSION);
	debug(5, "Copyright (C) 2010 %s\n", author);
	debug(5, "License GPLv2: GNU GPL version 2 <http://gnu.org/licenses/gpl.html>\n");
	debug(5, "This is free software: you are free to change and redistribute it.\n");
	debug(5, "There is NO WARRANTY, to the extent permitted by law.\n");
	debug(5, "\nWritten by %s (%s)\n", author, PACKAGE_BUGREPORT);
#ifdef PACKAGE_URL
	debug(5, "           %s\n", PACKAGE_URL);
#endif
	exit(0);
}

/* Display program help information */
void display_help()
{
	debug(5, "usage:  %s [options]\n\n", PACKAGE);
	debug(5, "where options include:\n");
	debug(5, "  --version              Print the version\n");
	debug(5, "  -h, --help             Print this message\n");
	debug(5, "  -v, --verbose [level]  Set the amount of information to display\n");
	debug(5, "                         If no optional level is given, increase by 1\n");
	debug(5, "\n");
	debug(5, "  -l, --list             List windows on the display and screen\n");
	debug(5, "  -i, --info             Display window or pointer information\n");
	debug(5, "  -s, --set              Set window or pointer location or visibility\n");
#ifdef HAVE_EASE_MULTI
	debug(5, "  -e, --ease             Ease the window or pointer into position\n");
#endif
	debug(5, "\n");
	debug(5, "  -d, --display host:dpy X server and display to connect to\n");
	debug(5, "  -c, --screen screen    Set the screen to use\n");
	debug(5, "  -p, --pointer          Run actions on the pointer\n");
	debug(5, "  -w, --id windowid      Run actions on the window specified by id\n");
	debug(5, "  -n, --name windowname  Run actions on the window with the specified name\n");
	debug(5, "  --root                 Run actions on the root window\n");
	debug(5, "  -g, --geometry         Set the geometry of the window or pointer\n");
	debug(5, "  -f, --fullscreen       Set the window to the size of the screen\n");
	debug(5, "  -t, --top              Set window to display on the top of all others\n");
	debug(5, "  -x, --hide             Hide the window or pointer\n");
	debug(5, "  -u, --unhide           Unhide the window or pointer\n");
	debug(5, "\n");
	debug(5, "examples:\n");
	debug(5, "  List windows:          xlayout --display :1.0 --list\n");
	debug(5, "                         xlayout -ld d01.localnet.com:0.0\n");
	debug(5, "                         xlayout -l\n");
	debug(5, "  Show win info:         xlayout --display :0.0 --info --id 0xa00001\n");
#ifdef HAVE_REGEX_H
	debug(5, "                         xlayout --info --name '[Ff]irefox' (regex example)\n");
#endif
	debug(5, "                         xlayout --info --name \"google.com - Mozilla Firefox\"\n");
	debug(5, "                         xlayout -iw 0xa00001\n");
	debug(5, "  Set window:            xlayout --set --name MPlayer --fullscreen\n");
#ifdef HAVE_REGEX_H
	debug(5, "                         xlayout -x --name \"^x(eyes|clock)\" (regex example)\n");
#endif
	debug(5, "                         xlayout --name MPlayer --hide --verbose 10\n");
	debug(5, "                         xlayout --display domain.com:0.0 --name MPlayer --top\n");
	debug(5, "                         xlayout -sn MPlayer -g 400x300+100+100\n");
#ifdef HAVE_EASE_MULTI
	debug(5, "                         xlayout -sen xawtv -g 500x400+200+100\n");
#endif
	debug(5, "  Show pointer:          xlayout -ip\n");
	debug(5, "                         xlayout --info --pointer\n");
	debug(5, "  Set pointer:           xlayout --set --pointer --geometry 0x0+100+100\n");
	debug(5, "                         xlayout -spg 0x0+100+100\n");
	debug(5, "                         xlayout -spx\n");
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
	(verbosity==1) ? debug(1,"\n") : (void)NULL;
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
	debug(5, "X: ");
	debug(1, "%d, ", x);
	debug(5, "Y: ");
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

#ifdef HAVE_EASE_MULTI
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
	debug(8, "Setting the pointer to hidden\n");

	/* Pointer window information */
	XLWindow w;
	w.id = "0";
	int x, y;
	unsigned int mask;

	/* Cursor information */
	Pixmap bm_no;
	Colormap cmap;
	Cursor no_ptr;
	XColor black, dummy;
	static char bm_no_data[] = {0, 0, 0, 0, 0, 0, 0, 0};

	/* Call XQueryPointer to get the current window the pointer is on */
	debug(10, "Getting pointers current window\n");
	create_window(&w);
	XQueryPointer(d.display, w.window, &w.window, &w.window, &x, &y, &x, &y, &mask);

	/* Create a transparent single pixel bitmap to be used as our cursor */
	cmap = DefaultColormap(d.display, DefaultScreen(d.display));
	XAllocNamedColor(d.display, cmap, "black", &black, &dummy);
	bm_no = XCreateBitmapFromData(d.display, w.window, bm_no_data, 8, 8);
	no_ptr = XCreatePixmapCursor(d.display, bm_no, bm_no, &black, &black, 0, 0);

	/* Map the transparent cursor to the current windows cursor */
	XDefineCursor(d.display, w.window, no_ptr);
	XFreeCursor(d.display, no_ptr);

	/* Cleanup our mess */
	if (bm_no != None)
		XFreePixmap(d.display, bm_no);
	XFreeColors(d.display, cmap, &black.pixel, 1, 0);

	debug(5, "Pointer hidden\n");
	return;
}

/* Show the pointer */
void show_pointer()
{
	debug(8, "Setting the pointer to shown\n");

	/* Cursor and window variables */
	Cursor csr;
	XLWindow w;
	w.id = "0";
	int x, y;
	unsigned int mask;

	/* Create a connection to the root window, then overwrite it with the */
	/* window the pointer is currently over */
	create_window(&w);
	XQueryPointer(d.display, w.window, &w.window, &w.window, &x, &y, &x, &y, &mask);

	/* Create the default cursor, then map it to the window */
	csr = XCreateFontCursor(d.display, XC_left_ptr);
	XDefineCursor(d.display, w.window, csr);
	XFreeCursor(d.display, csr);

	debug(5, "Pointer shown\n");
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
		debug(8, "Searching for window named %s\n", w->name);
		Window root;
		root = RootWindow(d.display,d.screen);
		w->window = create_window_named(w->name, root);
		debug(10, "window: 0x%lx\n", w->window);
	}

	/* We cant set the window, return an error */
	if (!w->window) {
		debug(5, "There was an error selecting the window!\n");
		exit(1);
	}
	return;
}

/* Select a window by the window name */
Window create_window_named(char *name, Window tmp_window)
{
	/* Define our window and class structures */
	Window *children, dummy, test_window;
	XClassHint tmp_cname;
	XTextProperty tmp_wmname;
	char str_wmname[100];
	char *tmp_name = NULL;
	char **cliargv = NULL;
	int cliargc;
	unsigned int child_count;

	/* Check if we have a match using the basic XFetchName */
	if (XFetchName(d.display, tmp_window, &tmp_name)) {
#ifdef HAVE_REGEX_H
		if (!regexec(&re, tmp_name, (size_t)0, NULL, 0)) {
#else
		if (strcmp(tmp_name, name) == 0) {
#endif
			debug(9, "Found %s in 0x%lx using XFetchName\n", name, tmp_window);
			return(tmp_window);
		} else {
			debug(9, "%s does not match XFetchName\n", tmp_name);
		}
	}

	/* Check if we have a match using the basic XGetWMName */
	if (XGetWMName(d.display, tmp_window, &tmp_wmname)) {
		sprintf(str_wmname, "%s", tmp_wmname.value);
#ifdef HAVE_REGEX_H
		if (!regexec(&re, str_wmname, (size_t)0, NULL, 0)) {
#else
		if (strcmp(str_wmname, name) == 0) {
#endif
			debug(9, "Found %s in 0x%lx using XGetWMName\n", name, tmp_window);
			return(tmp_window);
		} else {
			debug(9, "%s does not match XGetWMName\n", str_wmname);
		}
	}

	/* Check if we have a match from XGetClassHint for this window */
	if (XGetClassHint(d.display, tmp_window, &tmp_cname)) {
#ifdef HAVE_REGEX_H
		if (!regexec(&re, tmp_cname.res_name, (size_t)0, NULL, 0) || !regexec(&re, tmp_cname.res_class, (size_t)0, NULL, 0)) {
#else
		if ((strcmp(tmp_cname.res_name, name) == 0) || (strcmp(tmp_cname.res_class, name) == 0)) {
#endif
			debug(9, "Found %s in 0x%lx using class hints\n", name, tmp_window);
			return(tmp_window);
		} else {
			debug(9, "%s does not match XGetClassHint\n", tmp_cname.res_name);
		}

		/* Clean up our resources */
		XFree(tmp_cname.res_name);
		XFree(tmp_cname.res_class);
	}

	/* Check if we have a match using the basic XGetCommand */
	if (XGetCommand(d.display, tmp_window, &cliargv, &cliargc)) {
#ifdef HAVE_REGEX_H
		if (!regexec(&re, *cliargv, (size_t)0, NULL, 0)) {
#else
		if (strcmp(cliargv, name) == 0) {
#endif
			debug(9, "Found %s in 0x%lx using XGetCommand\n", name, tmp_window);
			return(tmp_window);
		} else {
			debug(9, "%s does not match XGetCommand\n", *cliargv);
		}
	}

	/* Fetch a list of children windows for the current window */
	if (!XQueryTree(d.display, tmp_window, &dummy, &dummy, &children, &child_count)) {
		debug(10, "0x%lx has no children\n", tmp_window);
		return(0);
	} else {
		debug(10, "Have %d children\n", child_count);
		while (child_count--) {
			test_window = create_window_named(name, children[child_count]);
			debug(10, "Checking if 0x%lx is a result with %d more children\n", test_window, child_count);
			if (test_window != 0) {
				debug(10, "tmp_window: 0x%lx\n", test_window);
				return(test_window);
				break;
			}
		}
	}

	//if (children) XFree ((char *)children);
	debug(10, "Got to the end\n");
	return(0);
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
	/* Try fetch the window attributes */
	XWindowAttributes winAttribs;
	if (XGetWindowAttributes(d.display, gw->window, &winAttribs) == 0) {
		/* We could not get the window attributes, report an error and exit program */
		debug(5, "failed to get window attributes\n");
		exit(1);
	}

	/* Set our windows attributes to those returned */
	gw->x		 		= winAttribs.x;
	gw->y		 		= winAttribs.y;
	gw->w		 		= winAttribs.width;
	gw->h		 		= winAttribs.height;
	gw->border			= winAttribs.border_width;

	/* Display debug information */
	debug(8, "Current window dimensions are x:%d, y:%d, w:%d, h:%d\n", gw->x, gw->y, gw->w, gw->h);
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
		XWindowAttributes window_attributes;
		if (XGetWindowAttributes(d.display, RootWindow(d.display,d.screen), &window_attributes) == 0) {
			debug(5, "Failed to get the root window attributes\n");
			exit(1);
		}
		rx 	= window_attributes.x;
		ry 	= window_attributes.y;
		rw 	= window_attributes.width;
		rh 	= window_attributes.height;
	}

	w->x = rx;
	w->y = ry;
	w->w = rw;
	w->h = rh;
	XMoveResizeWindow(d.display, w->window, w->x, w->y, w->w, w->h);
	return;
}

#ifdef HAVE_EASE_MULTI
/* Callback to set the window for easing */
int ease_window(Ease_Multi *e, va_list ap)
{
	char geometry[20];
	XLWindow w;
	w = va_arg(ap, XLWindow);

	sprintf(geometry, "%dx%d+%d+%d", e->dimension[2].value, e->dimension[3].value, e->dimension[0].value, e->dimension[1].value);
	usleep(10000);

	/* Set the window to the new geometry */
	w.new_geometry = geometry;
	set_window(&w);
	XFlush(d.display);
	return 0;
}
#endif

/* Return a list of the windows on the X11 server */
void list_windows(Window w, int depth)
{
	Window *children, dummy;
	unsigned int child_count;
	XQueryTree(d.display, w, &dummy, &dummy, &children, &child_count);

	char *tmp_name;
	char geometry[20];
	char **cliargv = NULL;
	int cliargc;
	XTextProperty machtp;
	int tmp_depth = depth;
	XWindowAttributes tmp_attr;
	XTextProperty tmp_wmname;
	XClassHint tmp_cname;

	while (child_count--) {
		tmp_name = NULL;
		tmp_cname.res_name = NULL;
		XFetchName(d.display, children[child_count], &tmp_name);
		XGetWMName(d.display, children[child_count], &tmp_wmname);
		XGetClassHint(d.display, children[child_count], &tmp_cname);
		XGetWindowAttributes(d.display, children[child_count], &tmp_attr);
		sprintf(geometry, "%dx%d+%d+%d",tmp_attr.width, tmp_attr.height, tmp_attr.x, tmp_attr.y);

		/* Get WM_MACHINE and WM_COMMAND */
		if (!XGetWMClientMachine(d.display, children[child_count], &machtp)) {
			machtp.value = NULL;
			machtp.encoding = None;
		}

		if (!XGetCommand(d.display, children[child_count], &cliargv, &cliargc)) {
			if (machtp.value) XFree ((char *) machtp.value);
			if (verbosity <= 7) {
				continue;
			}
		}

		/* Display padding in the front */
		if (tmp_depth > 0)
			while (tmp_depth--) printf(" ");

		debug(1, "0x%lx",children[child_count]);

		switch (tmp_attr.map_state)
		{
			case 0:
				debug(3, ", unmapped");
				break;
			case 1:
				debug(3, ", unviewable");
				break;
			case 2:
				debug(3, ", visible");
				break;
		}
		debug(4, ", %s", geometry);
		debug(2, ", \"%s (%s)\"", tmp_cname.res_name ? tmp_cname.res_name : "unknown", tmp_name ? tmp_name : "unknown");
		debug(1, "\n");

		XFree(tmp_name);

		/* Display the children windows */
		list_windows(children[child_count], depth+1);
	}

	XFree(children);
}

/* Show information about a window */
void display_window(XLWindow *w)
{
	/* Fetch the windows name */
	debug(8, "Fetching windows name for 0x%lx\n", w->window);
	char *tmp_name;
	char **cliargv = NULL;
	int cliargc;
	XTextProperty tmp_wmname;
	XClassHint cname;

	XFetchName(d.display, w->window, &tmp_name);
	XGetWMName(d.display, w->window, &tmp_wmname);
	w->name = (tmp_name) ? tmp_name : "Root";
	debug(8, "Windows name returned %s\n", w->name);

	/* Fetch the windows attributes */
	debug(8, "Fetching windows attributes for 0x%lx\n", w->name);
	XWindowAttributes window_attributes;
	if (XGetWindowAttributes(d.display,w->window, &window_attributes) == 0) {
		debug(5, "failed to get window attributes\n");
		exit(1);
	}

	/* Set our windows attributes to those returned */
	w->x		 		= window_attributes.x;
	w->y		 		= window_attributes.y;
	w->w		 		= window_attributes.width;
	w->h		 		= window_attributes.height;
	w->border			= window_attributes.border_width;

	/* Display name, id, and geometry information */
	debug(5, "Window Name:   %s\n", tmp_wmname.value);
	debug(5, "WM Name:       %s\n", tmp_wmname.value);
	debug(5, "Window Id:     0x%lx\n", w->window);
	XGetClassHint(d.display, w->window, &cname) ? debug(5, "Inst/Class:    %s/%s\n", cname.res_name, cname.res_class) : (void)NULL;
	XGetCommand(d.display, w->window, &cliargv, &cliargc) ? debug(5, "Command:       %s\n", *cliargv) : (void)NULL;
	debug(5, "geometry:      ");
	debug(1, "%dx%d+%d+%d\n", w->w, w->h, w->x, w->y);
	debug(5, "X Position:    %d\n", w->x);
	debug(5, "Y Position:    %d\n", w->y);
	debug(5, "Window Width:  %d\n", w->w);
	debug(5, "Window Height: %d\n", w->h);
	debug(5, "Border Width:  %d\n", w->border);

	/* Display the map state */
	switch (window_attributes.map_state)
	{
		case 0:
			debug(5, "Map state:     unmapped\n");
			break;
		case 1:
			debug(5, "Map state:     unviewable\n");
			break;
		case 2:
			debug(5, "Map state:     visible\n");
			break;
	}

	/* Display the class */
	switch (window_attributes.class)
	{
		case 0:
			debug(5, "Type:          InputOutput\n");
			break;
		case 1:
			debug(5, "Type:          InputOnly\n");
			break;
	}

	debug(5, "Overridable:   %s\n", window_attributes.override_redirect ? "false" : "true");
	return;
}

