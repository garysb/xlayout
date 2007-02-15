/* vim: set ts=4 sw=4 nowrap: */
/*
Copyright (C) 2007 gary@stroppytux.net.

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

/* Stop multiple inclusions */
#ifndef _HAVE_XLAYOUT_
#define _HAVE_XLAYOUT_

/* Define a structure to store all information needed */
#define _GNU_SOURCE
#define WAIT_DEFAULT	0
#define WAIT_TIMEOUT	15
#define VERBOSE_DEFAULT	0
#define STRINGLENGTH	256

/* Enumeration for toggling state */
enum toggle {UNSET=0, SET};											/* UNSET = 0, SET = 1 */

/* Program information */
typedef struct {
	char _name[50];													/* Hold the program name */
	char _author[50];												/* Program author */
	float _version;													/* Program version */
} aInfo;

/* Display information */
typedef struct {
	Display *_display;												/* Server or display to connect to */
	char _name[50];													/* The Display to use(localhost:0.0) */
	int _screen;													/* The screen to use on the display */
	int _screenCount;												/* Hold a list of the number of screens */
} dInfo;

/* Pointer information */
typedef struct {
	/* Pointer look */
	XColor _colour;													/* The colour for the cursor */
	Cursor _blank;													/* The blank cursor */
	Pixmap _pix;													/* The cursor pixmap */
	/* Pointer visibility state */
	enum toggle _visible;											/* pointer visibility enum */
	/* Pointer location */
	int _x;															/* Pointer x location */
	int _y;															/* Pointer y location */
	/* New pointer settings */
	char setGeometry[20];											/* Hold the new geometry */
} pInfo;

/* Window Information */
typedef struct {
	/* Window identification */
	Window _window;													/* The window to use */
	char _name[50];													/* The window name to show/set */
	char _id[20];													/* The window id to show/set */
	/* Window geometry */
	int _x;															/* Windows x location */
	int _y;															/* Windows y location */
	unsigned int _w;												/* Windows width */
	unsigned int _h;												/* Windows height */
	unsigned int _border;											/* Windows dorder width */
	/* Window visibility */
	enum toggle _visible;											/* Window visibility enum */
	enum toggle _top;												/* Window on top of all others */
	/* New window settings */
	char setGeometry[20];											/* Hold the new geometry */
	unsigned int setBorder;											/* New window border */
} wInfo;

/* Program Functions */
void version(aInfo *a);												/* Display version information */
void usage(aInfo *a);												/* Display usage options */
/* Display Functions */
void createDisplay(dInfo *d, aInfo *a);								/* Setup the display connection */
void destroyDisplay(dInfo *d, aInfo *a);							/* Close the connection to the display */
void listScreens(dInfo *d);											/* Return list of our screens */
/* Window Functions */
void selWindow(wInfo *w, aInfo *a, dInfo *d);						/* Select the window to show/set */
Window selWindowNamed(dInfo *d,char winsName[50],Window top);		/* Select a window with a -name */
void listWindows(dInfo *d);											/* Just list all windows info */
void setWindowTop(dInfo *d, wInfo *w);								/* Place the window ontop of all others */
void setWindowVisible(dInfo *d, wInfo *w);							/* Map the window and place it ontop of all others */
void setWindowHidden(dInfo *d, wInfo *w);							/* Hide the window */
void setWindowBorder(dInfo *d, wInfo *w);							/* Set the border width of a window */
void showWindow(aInfo *a, dInfo *d, wInfo *w);						/* Just show window information */
void setWindow(aInfo *a, dInfo *d, wInfo *w);						/* Set the window information */
//void setWindowEase(struct tag *pS);									/* Set the window with easing function */

/* Pointer Functions */
void showPointer(dInfo *d, wInfo *w);								/* Just show pointer information */
void setPointerHidden(dInfo *d, pInfo *p);							/* Hide the pointer */
void setPointer(dInfo *d, pInfo *p);								/* Set the pointer information */

#endif
