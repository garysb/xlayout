/* vim: set ts=4 sw=4 nowrap: */
/*
Copyright (C) 2007 Gary Stidston-Broadbent.

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

/******TODO******/
/* Display a more acurate list of window names */
/* Display list of screens */
/* Debug statements */
/* Valgrind the code */

/* Includes needed (These are limited to core X11 headers) */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>
#include <ease.h>
#include "xlayout.h"

/* Flags to set state */
static int verbose_flag;												/* Flag set by `--verbose' */
static int help_flag;													/* Display help */
static int version_flag;												/* Display version information */
static int pointer_flag;												/* Run action on pointer instead of window */
static int fscreen_flag;												/* Set window fullscreen */
static int top_flag;													/* Place window on top */
static int hidden_flag;													/* Set window or pointer to hidden state */
static int root_flag;													/* Run action on root window */
static int set_flag;													/* Set the window or pointer */
static int info_flag;													/* Show information about window or pointer */
static int show_flag;													/* Show information about window or pointer */
static int list_flag;													/* Reverse the order of the window list */
static int reverse_flag;												/* List all windows or screens */

int main(int argc, char **argv)											/* Main function */
{
	/* Preset variables */
	int c;																/* Option parse counter */
	unsigned int border;												/* Moves to w->_border */
	char geometry[20];													/* Moves to p->_geometry or w->_geometry */
	char display[50];													/* Moves to d->_name */
	int screen;															/* Moves to d->_screen */
	char id[20];														/* Moves to w->_id */
	char name[50];														/* Moves to w->_name */
	/* Flag settings */
	verbose_flag	= 0;												/* Disable verbosity */
	help_flag		= 0;												/* Disable help */
	version_flag	= 0;												/* Disable version information */
	pointer_flag	= 0;												/* Disable pointer actions */
	fscreen_flag	= 0;												/* Set window fullscreen */
	top_flag		= 0;												/* Set window on top */
	hidden_flag		= 0;												/* Hide pointer / window */
	root_flag		= 0;												/* Run actions on root window */
	set_flag		= 0;												/* Set window or pointer */
	info_flag		= 0;												/* Show window or pointer information */
	show_flag		= 0;												/* Show window or pointer information */
	list_flag		= 0;												/* List windows or screens */
	reverse_flag	= 0;												/* List windows in reverse */
	/* Tempory information holders to be moved out */
	border			= 99999;											/* Set the border to 99999 */
	strcpy(geometry,"NULL");											/* Set geometry to NULL */
	strcpy(id,"NULL");													/* Set id to NULL */
	strcpy(name,"NULL");												/* Set name to NULL */
#ifdef _HAVE_LIBEASE_
	int sEase;															/* Sets ease type */
	sEase			= 0;												/* Set sEase to NULL */
#endif
	/* Setup program information */
	aInfo a;															/* Create our program holder */
	strcpy(a._author,"Gary Broadbent");									/* Program Author */
	strcpy(a._name,argv[0]);											/* Program Name */
	a._version		= 0.8;												/* Program Version */
	while (1) {
		/* Start parsing command line options */
		static struct option long_options[] = {
			/* These options set a flag. */
			{"verbose",		no_argument,		&verbose_flag,	1},		/* We want to have verbose output */
			/* Display Options first */
			{"display",		required_argument,	0,				'd'},	/* Our display to use */
			{"screen",		required_argument,	0,				's'},	/* The screen to connect to */
			/* Information */
			{"help",		no_argument,		&help_flag,		1},		/* Display help */
			{"version",		no_argument,		&version_flag,	1},		/* Display version information */
			/* Window and pointer information */
			{"pointer",		no_argument,		&pointer_flag,	1},		/* Run actions on pointer, else window */
			{"fullscreen",	no_argument,		&fscreen_flag,	1},		/* Set window fullscreen */
			{"top",			no_argument,		&top_flag,		1},		/* Set window on top of all others */
			{"hidden",		no_argument,		&hidden_flag,	1},		/* Hide window or pointer */
			{"root",		no_argument,		&root_flag,		1},		/* Run actions on root window */
			{"geometry",	required_argument,	0,				'g'},	/* geometry (Used for window and pointer) */
			{"id",			required_argument,	0,				'i'},	/* The window ID */
			{"name",		required_argument,	0,				'n'},	/* The window name */
			{"border",		required_argument,	0,				'b'},	/* The window border */
#ifdef _HAVE_LIBEASE_
			{"ease",		required_argument,	0,				'e'},	/* Use / set the ease method */
#endif
			/* Actions */
			{"set",			no_argument,		&set_flag,		1},		/* Set window or pointer */
			{"info",		no_argument,		&info_flag,		1},		/* Show information on window oor pointer */
			{"show",		no_argument,		&show_flag,		1},		/* Map the window */
			{"list",		no_argument,		&list_flag,		1},		/* List windows or screens */
			{"reverse",		no_argument,		&reverse_flag,	1},		/* Reverse our window list */
			{0, 0, 0, 0}												/* Set to ZERO to detect end */
		};
		int option_index = 0;											/* getopt_long stores the option index here. */
		c = getopt_long(argc, argv, "d:s:g:i:n:b:",
						long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1) {
			break;
		}
		/* Run actions on our commands passed into the program */
		switch(c) {
			case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0) {
				break;
			}
			printf ("option %s", long_options[option_index].name);
			if (optarg)
				printf (" with arg %s", optarg);
			printf ("\n");
			break;

			case 'd':													/* Set the display */
				strcpy(display,optarg);									/* Copy from struct to temp name holder */
				break;
#ifdef _HAVE_LIBEASE_
			case 'e':													/* Set the ease method */
				sEase	= *optarg;										/* Copy from struct to temp name holder */
				break;
#endif
			case 's':													/* Set the screen */
				screen = atoi(optarg);									/* Copy from struct to temp name holder */
				break;
			case 'g':													/* Set the geometry */
				strcpy(geometry,optarg);								/* Copy from struct to temp name holder */
				break;
			case 'i':
				strcpy(id,optarg);										/* Copy from struct to temp name holder */
				break;
			case 'n':
				strcpy(name,optarg);									/* Copy from struct to temp name holder */
				break;
			case 'b':
				border	= atoi(optarg);									/* Copy the border to temp */
				break;
			case '?':
				/* getopt_long already printed an error message. */
				break;
			default:
				abort();												/* Something went wrong */
		}
	}

	/* Display help, then exit */
	if (help_flag) {
		usage(&a);														/* Display usage information */
		exit(0);														/* Exit successfully */
	}
	/* Display version, then exit */
	if (version_flag) {
		version(&a);													/* Display version information */
		exit(0);														/* Exit successfully */
	}
	/* If we need to do actions on the pointer, do them */
	if (pointer_flag) {
		pInfo p;														/* Create a struct for the pointer */
		dInfo d;														/* Create display information */
		d._screen = screen;												/* Copy tempory screen to display screen */
		strcpy(d._name,display);										/* Copy tempory display to main display */
		createDisplay(&d, &a);											/* Connect to the display */
		/* We either set or show the pointer */
		if (info_flag) {
			/* Open root window */
			wInfo w;													/* Create connection to window */
			strcpy(w._id,"0");
			selWindow(&w,&a,&d);										/* Connect to the root window */
			/* Show pointer */
			showPointer(&d,&w);											/* Show pointer information */
			/* Clear window */
		} else if (hidden_flag) {
			setPointerHidden(&d, &p);									/* Hide the pointer */
		} else if (set_flag) {
			/* Check to see if we have the geometry set */
			if (strcmp(geometry,"NULL")) {
				strcpy(p.setGeometry,geometry);							/* Copy the geometry */
#ifdef _HAVE_LIBEASE_
				/* Check if we can ease, ease the pointer into place */
				if (sEase) {
					Ease inEase;										/* Create our structure */
					inEase.type			= IN	;						/* Set our ease type. (IN,OUT,IO,OI,NONE) */
					inEase.initial		= 0;							/* Set our starting point */
					inEase.duration		= 50;							/* The quantity of times the easing should take to complete */
					inEase.difference	= 200;							/* Set the difference (eg.we want to move the object 10 px) */
					
				}
#endif
				setPointer(&d,&p);										/* Set the pointer */
			} else {
				usage(&a);												/* We didnt get a pointer, return usage */
			}
		}
		destroyDisplay(&d, &a);											/* Close the connection to the display */
	} else {															/* We dont use pointer, use window */
		/* Setup the display */
		dInfo d;														/* Create display information */
		d._screen = screen;												/* Copy tempory screen to display screen */
		strcpy(d._name,display);										/* Copy tempory display to main display */
		createDisplay(&d, &a);											/* Connect to the display */
		/* If we used the list flag */
		if (list_flag) {
			listWindows(&d);											/* List all our windows */
		} else {
			/* Setup the window */
			wInfo w;													/* Create a window struct */
			/* Choose the window to use */
			if (root_flag) {
				strcpy(w._id,"0");										/* Check if we used --root */
			} else if (strcmp(id,"NULL")) {								/* If not, check if we set an id */
				strcpy(w._id,id);										/* Copy the window id */
			} else if (strcmp(name,"NULL")) {							/* else sheck we used a name */
				strcpy(w._id,id);										/* Copy the window id */
				strcpy(w._name,name);									/* Copy the window name */
			}
			selWindow(&w,&a,&d);										/* Connect to the window */
			if (info_flag) {
				showWindow(&a,&d,&w);									/* Show information about a window */
			} else if (set_flag) {
				if (hidden_flag) {										/* Set the window to hidden */
					setWindowHidden(&d, &w);							/* Hide the window */
				} else if (show_flag) {
					setWindowVisible(&d, &w);							/* Map the window */
				} else if (top_flag) {
					setWindowTop(&d, &w);								/* Place the window ontop of all others */
				} else {
					if (strcmp(geometry,"NULL")) {
						strcpy(w.setGeometry,geometry);					/* Copy the geometry */
					}
					if (fscreen_flag) {
						strcpy(w.setGeometry,"fullscreen");				/* Copy fullscreen */
					}
					if (border != 99999) {
						w.setBorder	= border;							/* Set the new border */
						setWindowBorder(&d, &w);						/* Set the border */
					}
					setWindow(&a, &d, &w);
				}
			}
		}
		destroyDisplay(&d, &a);											/* Close the connection to the display */
	}
	/* Print any remaining command line arguments (not options). */
	if (optind < argc) {
		printf ("Please note that the following commands were not recognised: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		putchar ('\n');
	}
	exit (0);
}

/* Display program version information */
void version(aInfo *a)												/* Display version information */
{
	printf ("XLayout version %1.1f\n", a->_version);				/* Display the version number */
	printf ("Developed by %s\n", a->_author);						/* Display the author, ME! ;-) */
	putchar('\n');													/* Make our output tidy */
	exit(0);														/* Exit with success */
}
/* Display program usage information */
void usage(aInfo *a)												/* Display usage options */
{
	printf ("usage:  %s [-options ...]\n\n", a->_name);
	printf ("where options include:\n");
	printf ("    --version            Print the version\n");
	printf ("    --help               Print this message\n");
	printf ("    --list type          List either 'screens' or 'windows'\n");
	printf ("    --info               Display window information\n");
	printf ("    --set                Set window information\n");
	printf ("    --display host:dpy   X server to contact\n");
	printf ("    --root               Use the root window\n");
	printf ("    --id windowid        Use the window with the specified id\n");
	printf ("    --name windowname    Use the window with the specified name\n");
	printf ("    --pointer            Set/show the pointer instead of a window\n");
	printf ("    --geometry           Set the geometry of the id/name/pointer\n");
	printf ("    --fullscreen         Set window to fullscreen\n");
	printf ("    --border             Set window border in px\n");
	printf ("    --top                Set window to display on the top of all others\n");
	printf ("    --hidden             Hide the window or pointer\n");
	printf ("    --show               Unhide the window or pointer\n");
	printf ("    --verbose            Increase the verbosity of the output\n");
#ifdef _HAVE_LIBEASE_
	printf ("    --ease               Set ease method, can be 'in','out','io','oi'\n");
#endif
	printf ("    --reverse            If several windows match the name given with --name,\n");
	printf ("                         the default behavior is to select the bottom-most.\n");
	printf ("                         Use this option to select the top-most instead.\n");
	printf ("\n");
	printf ("examples:\n");
	printf ("    List windows:  xlayout --display :1.0 --list windows\n");
	printf ("    Show win info: xlayout --display :0.0 --show --id 0xa00001\n");
	printf ("                   xlayout --info --name \"www.google.co.uk - Mozilla Firefox\"\n");
	printf ("    Set window:    xlayout --set --name MPlayer --fullscreen\n");
#ifdef _HAVE_LIBEASE_
	printf ("                   xlayout --set --id 0x200034 --geometry 100x100-0-0 --ease oi\n");
#endif
	printf ("                   xlayout --set --name MPlayer --hidden\n");
	printf ("                   xlayout --set --display domain.com:0.0 --name MPlayer --top\n");
	printf ("    Show pointer:  xlayout --info --pointer --display :0.0\n");
	printf ("    Set pointer:   xlayout --set --pointer --geometry 0x0+100+100\n");
	printf ("\n");
	exit(0);
}
/* Create a connection to our display and screen */
void createDisplay(dInfo *d, aInfo *a)								/* Setup the display connection */
{
	if ((d->_display = XOpenDisplay(d->_name)) == NULL) {
		if (verbose_flag) {											/* If we need to display an error */
			printf ("%s: Unable to open display '%s'\n",
			a->_name, XDisplayName(d->_name));
		}
		exit(1);													/* Clean exit out of program */
	}
	d->_screen = DefaultScreen(d->_display);						/* Set the default screen(0) */
	return;															/* Return to main function */
}
/* Create a connection to our display and screen */
void destroyDisplay(dInfo *d, aInfo *a)								/* Setup the display connection */
{
	XFlush(d->_display);											/* Flush the display */
	XCloseDisplay(d->_display);										/* Now close the connection */
	return;															/* Return to main function */
}
/* Return the quantity of screens attached to the X server */
void listScreens(dInfo *d)											/* List all screens */
{
	d->_screenCount	= XScreenCount(d->_display);					/* Get the screen list */
	printf("There are %d screens.\n",d->_screenCount);				/* Print the result */
}

/* Create a connection to a window */
void selWindow(wInfo *w, aInfo *a, dInfo *d)						/* Select the window to show/set */
{
	if (!strcmp(w->_id, "0")) {										/* If the windowId has been set to 0 */
		w->_window = 0;
		w->_window=RootWindow(d->_display,d->_screen);				/* Set win to root window */
		return;														/* Return to Main function */
	}
	if (strcmp(w->_id, "NULL")) {									/* If the windowId has been set */
		w->_window = 0;												/* Blank the window */
		sscanf(w->_id, "0x%lx", &w->_window);						/* Check the format is correct */
		if (!w->_window)											/* If not */
			sscanf(w->_id, "%ld", &w->_window);						/* Try different format */
		if (!w->_window) {											/* If still not */
			if (verbose_flag) {										/* If we need verbose output */
				printf("Invalid window id format: %s.\n",			/* Print an error message saying invalid id */
				w->_id);
			}
			exit(1);												/* Exit program */
		} else {
			w->_window = XmuClientWindow(d->_display, w->_window);
			return;													/* Else exit function */
		}
	}
	if (strcmp(w->_name, "NULL")) {									/* If -name was set */
		Window top;													/* Create reference to root window */
		char winName[50];											/* Create a tempory name holder */
		char tmpId[20];												/* Used to display id at end */
		strcpy(winName,w->_name);									/* Copy from struct to temp name holder */
		top = RootWindow(d->_display,d->_screen);					/* Set top to the root of the display */
		w->_window = selWindowNamed(d,winName,top);					/* Set win to the id returned by selWindowNamed */
		sprintf(tmpId, "0x%lx", w->_window);						/* Convert from dec to hex */
		strcpy(w->_id,tmpId);										/* Copy to pS struct */
	}
	if (!w->_window) {												/* If win still doesnt exist */
		if (verbose_flag) {											/* If we should display our error */
			printf("There was an error selecting the window!\n");
		}
		exit(1);													/* Exit the program */
	}
	return;															/* Return to main */
}
/* Select a window by the window name */
Window selWindowNamed(dInfo *d, char winsName[50], Window top)		/* Select a window with a -name */
{
	Window *children, dummy;										/* Set tempory holders */
	unsigned int nchildren;											/* Set integer holders */
	register int j;													/* Store a non memory int */
	char *window_name;												/* Create a return char */
	if (XFetchName(d->_display, top, &window_name) && strcmp(window_name, winsName) == 0) {
	  return(top);													/* Return the root window */
	}
	if (!XQueryTree(d->_display, top, &dummy, &dummy, &children, &nchildren)) {
		return(0);													/* Return no window */
	}
	/* Return results on creation order */
	if(!reverse_flag) {
		for (j=0; j<nchildren; j++) {								/* Run loop of windows/children */
			top = selWindowNamed(d,winsName,children[j]);			/* Recall this function (LOOP) */
			if (top)												/* If it finds the win */
			  break;												/* Break the loop */
		}
	/* Return the results from newest to oldest creation */
	} else {
		for (j=nchildren-1; j >= 0; j--) {							/* Run loop of windows/children */
			top = selWindowNamed(d,winsName,children[j]);			/* Recall this function (LOOP) */
			if (top)												/* If it finds the win */
				break;												/* Break the loop */
		}
	}
	if (children) XFree ((char *)children);							/* Clear up our mess */
	return(top);													/* Return the window id */
}
/* Return a list of the windows on the X11 server */
void listWindows(dInfo *d)											/* List all windows */
{
	Window *children, dummy;										/* Set tempory holders */
	unsigned int nchildren;											/* Set integer holders */
	register int k;													/* Store a non memory int */
	XQueryTree(d->_display,											/* Query tree from root window */
		RootWindow(d->_display,d->_screen),
		&dummy,
		&dummy,
		&children,
		&nchildren);
	if (verbose_flag) {
		printf("ID			NAME\n\n");								/* Print out heading */
	}
/*FIXME*/
//	if (1) {														/* Check if we want the wm name */
//		XTextProperty tmpName;										/* Create a text property */
//		for (k=0; k<nchildren; k++) {								/* Run loop of windows/children */
//			printf("0x%lx",children[k]);							/* Print out the window id */
//			tmpName.value = NULL;									/* Clear out the name */
//			XGetWMName(d->_display, children[k], &tmpName);				/* Fetch the tempory name */
//			if ( tmpName.value ) {									/* If a name has been set */
//				printf("		%s\n",tmpName.value);				/* Copy temp name to perm name */
//			} else {												/* Else set name */
//				printf("		Not Available\n");					/* Copy temp name to perm name */
//			}
//		}
//	} else {
		if (!reverse_flag) {
			char *tmpName;											/* Tempory name before copied to pS->windowName */
			for (k=0; k<nchildren; k++) {							/* Run loop of windows/children */
				tmpName = NULL;										/* Clear out the name */
				XFetchName(d->_display, children[k], &tmpName);		/* Fetch the tempory name */
				if (tmpName) {										/* If a name has been set */
					printf("0x%lx",children[k]);					/* Print out the window id */
					printf("		%s\n",tmpName);					/* Copy temp name to perm name */
				} else {											/* Else set name */
					if (verbose_flag) {
						printf("0x%lx",children[k]);				/* Print out the window id */
						printf("		Not Available\n");			/* Copy temp name to perm name */
					}
				}
			}
		} else {
			char *tmpName;											/* Tempory name before copied to pS->windowName */
			for (k=nchildren-1; k>=0; k--) {						/* Run loop of windows/children */
				tmpName = NULL;										/* Clear out the name */
				XFetchName(d->_display, children[k], &tmpName);		/* Fetch the tempory name */
				if (tmpName) {										/* If a name has been set */
					printf("0x%lx",children[k]);					/* Print out the window id */
					printf("		%s\n",tmpName);					/* Copy temp name to perm name */
				} else {											/* Else set name */
					if (verbose_flag) {
						printf("0x%lx",children[k]);				/* Print out the window id */
						printf("		Not Available\n");			/* Copy temp name to perm name */
					}
				}
			}
		}
//	}
}
/* Show information about a window */
void showWindow(aInfo *a, dInfo *d, wInfo *w)						/* Just show window information */
{
	char *tmpName;													/* Tempory name before copied to pS->windowName */
	XFetchName(d->_display, w->_window, &tmpName);					/* Fetch the tempory name */
    XWindowAttributes winAttribs;									/* define struct */
	if ( XGetWindowAttributes(d->_display,w->_window,&winAttribs) == 0) {
		if (verbose_flag) {											/* If -quiet called use int to display error */
			printf("%s: failed to get window attributes\n",a->_name);
		}
		exit(1);													/* Break out of program */
	}
	if ( tmpName ) {												/* If a name has been set */
		strcpy(w->_name,tmpName);									/* Copy temp name to perm name */
	} else {														/* Else set name */
		strcpy(w->_name, "Root");									/* pS->windowname to Root window */
	}
	w->_x		 		= winAttribs.x;								/* Store the X value */
	w->_y		 		= winAttribs.y;								/* Store the Y value */
	w->_w		 		= winAttribs.width;							/* Store the Width value */
	w->_h		 		= winAttribs.height;						/* Store the Height value */
	w->_border			= winAttribs.border_width;					/* Store the border width */
	printf("Window Name:   %s\n",w->_name);							/* Print out the window name */
	printf("Window Id:     %s\n", w->_id);							/* Print the ID chosen */
	printf("geometry:      %dx%d+%d+%d\n",							/* Print the 'geometry */
		w->_w,
		w->_h,
		w->_x,
		w->_y);
	printf("X Position:    %d\n",w->_x);							/* Print out the X value */
	printf("Y Position:    %d\n",w->_y);							/* Print out the Y value */
	printf("Window Width:  %d\n",w->_w);							/* Print out the width */
	printf("Window Height: %d\n",w->_h);							/* Print out the height */
	printf("Border Width:  %d\n",w->_border);						/* Print out the height */
	return;															/* return to main function */
}
/* Map the window for view */
void setWindowVisible(dInfo *d, wInfo *w)
{
	XMapWindow(d->_display,w->_window);								/* Map the window */
	XRaiseWindow(d->_display,w->_window);							/* Place the window ontop of all others */
	return;
}
/* Set window ontop of all others */
void setWindowTop(dInfo *d, wInfo *w)
{
	XMapWindow(d->_display,w->_window);								/* First make sure the window is mapped */
	XRaiseWindow(d->_display,w->_window);							/* Raise the window to the top */
	return;
}
/* Hide the window from view */
void setWindowHidden(dInfo *d, wInfo *w)
{
	XUnmapWindow(d->_display,w->_window);							/* Hide the window */
	return;
}
/* Set the window border */
void setWindowBorder(dInfo *d, wInfo *w)
{
	XSetWindowBorderWidth(d->_display, w->_window,					/* Set the border width */
		w->setBorder);
	w->_border = w->setBorder;										/* Copy the new value into our struct */
	return;
}
/* Set the window position */
void setWindow(aInfo *a, dInfo *d, wInfo *w)						/* Set the window information */
{
	int rx, ry, rw, rh, rg;											/* Set some ints for return of XWMGeometry */
	XSizeHints hints = {0};											/* Set some hint defaults */
	if (strcmp(w->setGeometry, "fullscreen")) {						/* If -fullscreen wasn't entered */
		XWMGeometry(d->_display, d->_screen,						/* Get info from -geometry */
					w->setGeometry, "200x200+0+0",
					0, &hints, &rx, &ry, &rw, &rh, &rg);
	} else {														/* No -fullscreen */
		XWindowAttributes rootWinAttribs;							/* Define root struct */
		if ( XGetWindowAttributes(d->_display,						/* Get info from root window */
				RootWindow(d->_display,d->_screen),
				&rootWinAttribs) == 0) {							/* There was an error */
			if (verbose_flag) {										/* If -quiet called use int to display error */
				printf("%s: failed to get window attributes\n",a->_name);
			}
			exit(1);												/* Break out of program */
		}
		rx 	= rootWinAttribs.x;										/* Store the X value */
		ry 	= rootWinAttribs.y;										/* Store the Y value */
		rw 	= rootWinAttribs.width;									/* Store the Width value */
		rh 	= rootWinAttribs.height;								/* Store the Height value */
	}
	w->_x = rx;														/* Copy new X to struct */
	w->_y = ry;														/* Copy new Y to struct */
	w->_w = rw;														/* Copy new W to struct */
	w->_h = rh;														/* Copy new H to struct */
	XMoveResizeWindow(d->_display, w->_window,						/* Resize and move window */
		w->_x,
		w->_y,
		w->_w,
		w->_h);
	return;															/* Return to Main function */
}
/* Output information about the pointer */
void showPointer(dInfo *d, wInfo *w)								/* Show pointer information */
{
	int x, y, dummy;												/* X and Y and a dummy for crap */
	XQueryPointer (d->_display,										/* Return results of pointer */
		w->_window,
		&w->_window,
		&w->_window,
		&x, &y, &dummy, &dummy, &dummy);
	printf ("Pointer coordinates\n");								/* Header */
	printf ("X Position: %d\n", x);									/* X Coords */
	printf ("Y Position: %d\n", y);									/* Y Coords */
	return;															/* Exit function */
}
/* Hide the pointer */
void setPointerHidden(dInfo *d, pInfo *p)
{
	p->_pix = XCreatePixmap(
		d->_display,
		DefaultRootWindow(d->_display),
		1,
		1,
		1);
	p->_colour.pixel = 0;
	p->_colour.red = 0;
	p->_colour.green = 0;
	p->_colour.blue = 0;
	p->_colour.flags = 0;
	p->_colour.pad = 0;
	p->_blank = XCreatePixmapCursor(
		d->_display,
		p->_pix,
		p->_pix,
		&p->_colour,
		&p->_colour,
		1,
		1);
	XDefineCursor(d->_display,
		DefaultRootWindow(d->_display),
		p->_blank);
	XFlush(d->_display);
	return;
}
/* Set the location of the pointer */
void setPointer(dInfo *d, pInfo *p)										/* Set the pointer location */
{
	Window top;															/* Create a blank window */
	int x, y, w, h, g;													/* Set some ints for return of XWMGeometry */
	top = 0;															/* Blank win */
	top = DefaultRootWindow(d->_display);								/* Set win for pointer dpy */
	XSizeHints hints = {0};												/* Set some hint defaults */
	XWMGeometry(d->_display, d->_screen,								/* Get info from -geometry */
		p->setGeometry, "100x100+0+0",
		0, &hints, &x, &y, &w, &h, &g);
	p->_x = x;															/* Copy new X to struct */
	p->_y = y;															/* Copy new Y to struct */
	XWarpPointer(d->_display, top, top,									/* Set pointer position */
		0, 0, 0, 0, x, y);
	XFlush(d->_display);												/* Flush the display to make the changes take affect */
}
