/*
Copyright (C) 2004 Stroppytux

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
/* More easing methods, and maybe smooth the current ones */
/* Test the code thuroughly */

/* Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>
/* Define a structure to store all information needed */

#define WAIT_DEFAULT	0
#define WAIT_TIMEOUT	15
#define VERBOSE_DEFAULT	0
#define STRINGLENGTH	256

struct tag {
	char programMade[50];											/* Program Designer */
	char programName[50];											/* Program executed(argv[0]) */
	char programVersion[20];										/* Version Number */
	char displayName[50];											/* The Display to use(localhost:0.0) */
	int displayScreen;												/* The screen to use on the display */
	int screenCount;												/* Hold a list of the number of screens */
	char windowName[50];											/* The window name to show/set */
	char windowId[20];												/* The window id to show/set */
	int windowRoot;													/* Set if argv[] has --root */
	int useWM;														/* Set if argv[] has --wm */
	char typeList[10];												/* Set if argv[] has --list */
	int windowSet;													/* Set if argv[] has --set */
	int windowShow;													/* Set if argv[] has --show */
	char geometry[20];												/* Set if -geometry exists */
	int geometrySet;												/* If -geometry has been set */
	int pointerSet;													/* Set if argv[] has -pointer */
	int quietSet;													/* Display output messages */
	int quietBit;													/* Display output bit at end */
	int easeSet;													/* If easing is used */
	char ease[5];													/* Ease method */
	char easeD[1];													/* Ease X/Y/W/H */
	int duration;													/* The duration time for easing */
	int cycTime;													/* The cycle time */
	int windowX;													/* Window X location */
	int windowY;													/* Window Y location */
	int windowXInit;												/* Window X initial location */
	int windowYInit;												/* Window Y initial location */
	int windowXDif;													/* Difference in X initial location */
	int windowYDif;													/* Difference in Y initial location */
	int windowTop;													/* Window on top */
	int windowHidden;												/* Window hidden */
	unsigned int windowW;											/* Window Width */
	unsigned int windowH;											/* Window Height */
	unsigned int windowWInit;										/* Window Width */
	unsigned int windowHInit;										/* Window Height */
	unsigned int windowWDif;										/* Window Width Dif */
	unsigned int windowHDif;										/* Window Height Dif */
	unsigned int windowBorder;										/* The border width */
};

Display *display;													/* Server or display to connect to */
Window win,top;														/* The window to use */

struct tag plasmaStruct;											/* Define the srtucture */
void version(struct tag *pS);										/* Display version information */
void usage(struct tag *pS);											/* Display usage options */
void selDisplay(struct tag *pS);									/* Setup the display connection */
void selWindow(struct tag *pS);										/* Select the window to show/set */
Window selWindowNamed();											/* Select a window with a -name */
void showPointer(struct tag *pS);									/* Just show pointer information */
void showWindow(struct tag *pS);									/* Just show window information */
void listWindows(struct tag *pS);									/* Just list all windows info */
void listScreens(struct tag *pS);									/* Just list our screens */
void setWindow(struct tag *pS);										/* Set the window information */
void setWindowEase(struct tag *pS);									/* Set the window with easing function */
void setPointer(struct tag *pS);									/* Set the pointer information */
int easeIn(struct tag *pS);											/* Ease in function */
int easeOut(struct tag *pS);										/* Ease out function */
int easeIo(struct tag *pS);											/* Ease in/out function */
int easeOi(struct tag *pS);											/* Ease out/in function */

int main(int argc, char **argv)										/* Main function */
{
	struct tag *pS;													/* Create pointer type struct */
	pS = &plasmaStruct;												/* Set pS to memory of plasmaStruct */
	strcpy(pS->programMade,"Gary Broadbent");						/* Program Designer default */
	strcpy(pS->programName,"Default");								/* Default program name */
	strcpy(pS->programVersion,"0.7");								/* Program Version */
	strcpy(pS->displayName,":0.0");									/* Default display */
	strcpy(pS->windowName,"NULL");								 	/* Blank window name */
	strcpy(pS->windowId,"NULL");									/* Blank window id */
	strcpy(pS->geometry,"640x480+10+10");							/* Set the default geometry */
	pS->useWM			= 0;										/* Dissable the use of window manager */
	pS->screenCount		= 0;										/* Set to cound screens */
	pS->geometrySet		= 0;										/* Disable geometry setting */
	pS->pointerSet		= 0;										/* Disable pointer setting */
	pS->quietSet		= 0;										/* Disable messages setting */
	pS->quietBit		= 0;										/* Set to non 0 if error */
	pS->windowRoot		= 0;										/* Disable window root */
	strcpy(pS->typeList,"blank");									/* Display window as default */
	pS->windowSet		= 0;										/* Disable window set */
	pS->windowShow		= 0;										/* Disable window show */
	pS->easeSet			= 0;										/* Disable easing */
	strcpy(pS->ease,"in");											/* Default ease method */
	strcpy(pS->easeD,"y");											/* Default ease Diff */
	pS->duration		= 200;										/* Time it takes to ease */
	pS->cycTime			= 0;										/* Increased every cycle */
	pS->windowX			= 0;										/* Blank window X position */
	pS->windowY			= 0;										/* Blank window Y position */
	pS->windowW			= 0;										/* Blank window W position */
	pS->windowH			= 0;										/* Blank window H position */
	pS->windowXInit		= 0;										/* Blank window X position */
	pS->windowYInit		= 0;										/* Blank window Y position */
	pS->windowWInit		= 0;										/* Blank window W position */
	pS->windowHInit		= 0;										/* Blank window H position */
	pS->windowXDif		= 0;										/* Dif in win X */
	pS->windowYDif		= 0;										/* Dif in win Y */
	pS->windowWDif		= 0;										/* Dif in win W */
	pS->windowHDif		= 0;										/* Dif in win H */
	pS->windowBorder	= 0;										/* Set the border to 0 px */
	pS->windowTop		= 0;										/* Place window on top */
	pS->windowHidden	= 0;										/* Hide the window */
	strcpy(pS->programName,argv[0]);								/* Set name to execute name */

	/* First we set our default action to take if an action wasnt
	   issued on the command line. We have set this to list all
	   windows for now, but propably should be set to display help */
	if (argc < 2) {													/* If we have no arguments */
		strcpy(pS->typeList,"windows");								/* Set struct.typeList to windows */
	}
	register int i;													/* Define a non-memory int for counter */
	for (i = 1; i < argc; i++) {									/* Loop through arguments */
		if (!strcmp(argv[i], "--help")) {							/* If we have a -help argument */
			usage(pS);												/* Call usage function */
			continue;												/* And break loop */
		}
		if (!strcmp(argv[i], "--version")) {						/* If we have a -version argument */
			version(pS);											/* Call version function */
			continue;												/* And break loop */
		}
		if (!strcmp(argv[i], "--display")) {						/* If -display has been specified */
			if (++i >= argc){										/* But no value entered */
				usage(pS);											/* Call usage function */
			}
			strcpy(pS->displayName,argv[i]);						/* Set struct.displayName to cl input */
		}
//		if (!strcmp(argv[i], "--screens")) {						/* If --screen has been specified */
//			pS->screenList		= 1;								/* Enable screens list */
//		}
		if (!strcmp(argv[i], "--wm")) {								/* If --wm has been specified */
			pS->useWM		= 1;									/* Enable window manager hints */
		}
		if (!strcmp(argv[i], "--name")) {							/* If -name has been specified */
			if (++i >= argc){										/* But no value entered */
				usage(pS);											/* Call usage function */
			}
			strcpy(pS->windowName,argv[i]);							/* Set struct.windowName to cl input */
		}
		if (!strcmp(argv[i], "--id")) {								/* If -id has been specified */
			if (++i >= argc){										/* But no value entered */
				usage(pS);											/* Call usage function */
			}
			strcpy(pS->windowId,argv[i]);							/* Set struct.windowId to cl input */
		}
		if (!strcmp(argv[i], "--root")) {							/* If -root has been specified */
			pS->windowRoot		= 1;								/* Enable window root */
		}
		if (!strcmp(argv[i], "--geometry")) {						/* If -geometry has been specified */
			if (++i >= argc){										/* But no value entered */
				usage(pS);											/* Call usage function */
			}
			strcpy(pS->geometry,argv[i]);							/* Set struct.geometry to cl input */
			pS->geometrySet		= 1;								/* Enable Setup of geometry */
		}
		if (!strcmp(argv[i], "--border")) {							/* If -border has been specified */
			if (++i >= argc){										/* But no value entered */
				usage(pS);											/* Call usage function */
			}
			sscanf(argv[i],"%d",&pS->windowBorder);					/* Set struct.windowBorder to cl input */
		}
		if (!strcmp(argv[i], "--fullscreen")) {						/* If -fullscreen has been specified */
			strcpy(pS->geometry,"fullscreen");						/* set geometry to fullscreen */
		}
		if (!strcmp(argv[i], "--set")) {							/* If -set has been specified */
			pS->windowSet		= 1;								/* Enable Setup of window */
		}
		if (!strcmp(argv[i], "--show")) {							/* If -show has been specified */
			pS->windowShow		= 1;								/* Enable Show of window */
		}
		if (!strcmp(argv[i], "--pointer")) {						/* If -pointer has been specified */
			pS->windowRoot		= 1;								/* Enable window root */
			pS->pointerSet		= 1;								/* Enable pointer set/show */
		}
		if (!strcmp(argv[i], "--list")) {							/* If --list has been specified */
			if (++i >= argc){										/* But no value entered */
				usage(pS);											/* Call usage function */
			}
			strcpy(pS->typeList,argv[i]);							/* Set struct.typeList to cl input */
		}
		if (!strcmp(argv[i], "--top")) {							/* If -top has been specified */
			pS->windowTop		= 1;								/* Place window on top of all others */
		}
		if (!strcmp(argv[i], "--hidden")) {							/* If -hidden has been specified */
			pS->windowHidden	= 1;								/* Hide the window */
		}
		if (!strcmp(argv[i], "--quiet")) {							/* If -quiet has been specified */
			pS->quietSet		= 1;								/* dissable display of error messages */
		}
		if (!strcmp(argv[i], "--ease")) {							/* If -ease has been specified */
			if (++i >= argc){										/* But no value entered */
				usage(pS);											/* Call usage function */
			}
			pS->easeSet	= 1;										/* Use easing */
			strcpy(pS->ease,argv[i]);								/* Set struct.ease to cl input */
		}
	}
	/* Now start executing the functions */
	selDisplay(pS);													/* Select the display */
	if (strcmp(pS->typeList,"blank")) {								/* If we have issued a list */
		if (!strcmp(pS->typeList, "screens")) {						/* If -list, then list the screens */
			listScreens(pS);										/* Call list finction */
			exit(0);												/* Exit program */
		}
		if (!strcmp(pS->typeList, "windows")) {						/* If -list, then list the windows */
			listWindows(pS);										/* Call list finction */
			exit(0);												/* Exit program */
		}
	}
	selWindow(pS);													/* Select the window */
	if ( pS->windowSet ) {											/* If -set, then set window/pointer */
		if ( pS->pointerSet ) {										/* If showing pointer */
			setPointer(pS);											/* Call setPointer */
			exit(0);												/* And exit program */
		} else {													/* Else */
			if ( pS->easeSet ) {									/* If use easing */
				setWindowEase(pS);									/* Set window with easing */
			} else {
				setWindow(pS);										/* Set window */
			}
		}
	}
	if ( pS->windowShow ) {											/* If -show, then show the window/pointer */
		if ( pS->pointerSet ) {										/* If showing pointer */
			showPointer(pS);										/* Call showPointer */
			exit(0);												/* And exit program */
		} else {													/* Else */
			showWindow(pS);											/* Show window information */
		}
	}
	XCloseDisplay(display);											/* Tidy up Xserver connection */
	return(0);														/* Return out of main function(END PROG) */
}
void version(struct tag *pS)										/* Display version information */
{
	printf ("xlayout version %s\n", pS->programVersion);
	printf ("Developed by %s\n", pS->programMade);
	printf ("\n");
	exit(0);
}
void usage(struct tag *pS)											/* Display usage options */
{
	printf ("usage:  %s [-options ...]\n\n", pS->programName);
	printf ("where options include:\n");
	printf ("    --version            Print the version\n");
	printf ("    --help               Print this message\n");
	printf ("    --list type          List either 'screens' or 'windows'\n");
	printf ("    --show               Show window information\n");
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
	printf ("    --hidden             Hide the window\n");
	printf ("    --quiet              Return only 1 for success or 0 for error\n");
	printf ("    --ease               Set ease method, can be 'in','out','io','oi'\n");
	printf ("\n");
	printf ("examples:\n");
	printf ("    List windows:  xlayout --display :1.0 --list windows\n");
	printf ("    Show win info: xlayout --display :0.0 --show --id 0xa00001\n");
	printf ("                   xlayout --show --name \"www.google.co.uk - Mozilla Firefox\"\n");
	printf ("    Set window:    xlayout --set --name MPlayer --fullscreen\n");
	printf ("                   xlayout --set --id 0x200034 --geometry 100x100-0-0 --ease oi\n");
	printf ("                   xlayout --set --name MPlayer --hidden\n");
	printf ("                   xlayout --set --display domain.com:0.0 --name MPlayer --top\n");
	printf ("    Show pointer:  xlayout --show --pointer --display :0.0\n");
	printf ("    Set pointer:   xlayout --set --pointer --geometry 0x0+100+100\n");
	printf ("\n");
	exit(0);
}
void selDisplay(struct tag *pS)										/* Setup the display connection */
{
	if ((display = XOpenDisplay(pS->displayName)) == NULL) {
		if ( !pS->quietSet ) {										/* If -quiet called use int to display error */
			printf ("%s: Unable to open display '%s'\n",
			pS->programName, XDisplayName (pS->displayName));
		}
		exit(1);													/* Clean exit out of program */
	}
	pS->displayScreen = DefaultScreen(display);						/* Set the default screen(0) */
	return;															/* Return to main function */
}
void selWindow(struct tag *pS)										/* Select the window to show/set */
{
	if ( pS->windowRoot ) {											/* If -root called use root window */
		win = 0;
		win=RootWindow(display,pS->displayScreen);					/* Set win to root window */
		return;														/* Return to Main function */
	}
	if (strcmp(pS->windowId, "NULL")) {								/* If the windowId has been set */
		win = 0;													/* Blank the window */
		sscanf(pS->windowId, "0x%lx", &win);						/* Check the format is correct */
		if (!win)													/* If not */
			sscanf(pS->windowId, "%ld", &win);						/* Try different format */
		if (!win) {													/* If still not */
			if ( !pS->quietSet ) {									/* If -quiet called use int to display error */
				printf("Invalid window id format: %s.\n",			/* Print an error message saying invalid id */
				pS->windowId);
			}
			exit(1);												/* Exit program */
		} else {
			win = XmuClientWindow(display, win);
			return;													/* Else exit function */
		}
	}
	if (strcmp(pS->windowName, "NULL")) {							/* If -name was set */
		char winName[50];											/* Create a tempory name holder */
		char tmpId[20];												/* Used to display id at end */
		strcpy(winName,pS->windowName);								/* Copy from struct to temp name holder */
		top = RootWindow(display,pS->displayScreen);				/* Set top to the root of the display */
		win = selWindowNamed(winName,top);							/* Set win to the id returned by selWindowNamed */
		sprintf(tmpId, "0x%lx", win);								/* Convert from dec to hex */
		strcpy(pS->windowId,tmpId);									/* Copy to pS struct */
	}
	if (!win) {														/* If win still doesnt exist */
		if ( !pS->quietSet ) {										/* If -quiet called use int to display error */
			printf("There was an error selecting the window!\n");
		}
		exit(1);													/* Exit the program */
	}
	return;															/* Return to main */
}
Window selWindowNamed(winsName,top)									/* Select a window with a -name */
char winsName[50];													/* define a char for storing the name */
{
	Window *children, dummy;										/* Set tempory holders */
	unsigned int nchildren;											/* Set integer holders */
	register int j;													/* Store a non memory int */
	char *window_name;												/* Create a return char */
	if (XFetchName(display, top, &window_name) && !strcmp(window_name, winsName)){
	  return(top);													/* Return the root window */
	}
	if (!XQueryTree(display, top, &dummy, &dummy, &children, &nchildren))
	  return(0);													/* Return no window */
	for (j=0; j<nchildren; j++) {									/* Run loop of windows/children */
		win = selWindowNamed(winsName,children[j]);					/* Recall this function (LOOP) */
		if (win)													/* If it finds the win */
		  break;													/* Break the loop */
	}
	if (children) XFree ((char *)children);							/* Clear up our mess */
	return(win);													/* Return the window id */
}
void listScreens(struct tag *pS)									/* List all screens */
{
	pS->screenCount	= XScreenCount(display);						/* Get the screen list */
	printf("There are %d screens.\n",pS->screenCount);				/* Print the result */
}
void listWindows(struct tag *pS)									/* List all windows */
{
	Window *children, dummy;										/* Set tempory holders */
	unsigned int nchildren;											/* Set integer holders */
	register int k;													/* Store a non memory int */
	XQueryTree(display,												/* Query tree from root window */
		RootWindow(display,pS->displayScreen),
		&dummy,
		&dummy,
		&children,
		&nchildren);
	printf("ID			NAME\n\n");									/* Print out heading */
	if (pS->useWM ) {												/* Check if we want the wm name */
		XTextProperty tmpName;										/* Create a text property */
		for (k=0; k<nchildren; k++) {								/* Run loop of windows/children */
			printf("0x%lx",children[k]);							/* Print out the window id */
			tmpName.value = NULL;									/* Clear out the name */
			XGetWMName(display, children[k], &tmpName);				/* Fetch the tempory name */
			if ( tmpName.value ) {									/* If a name has been set */
				printf("		%s\n",tmpName.value);				/* Copy temp name to perm name */
			} else {												/* Else set name */
				printf("		Not Available\n");					/* Copy temp name to perm name */
			}
		}
	} else {
		char *tmpName;												/* Tempory name before copied to pS->windowName */
		for (k=0; k<nchildren; k++) {								/* Run loop of windows/children */
			printf("0x%lx",children[k]);							/* Print out the window id */
			tmpName = NULL;											/* Clear out the name */
			XFetchName(display, children[k], &tmpName);				/* Fetch the tempory name */
			if ( tmpName ) {										/* If a name has been set */
				printf("		%s\n",tmpName);						/* Copy temp name to perm name */
			} else {												/* Else set name */
				printf("		Not Available\n");					/* Copy temp name to perm name */
			}
		}
	}
}
void showPointer(struct tag *pS)									/* Show pointer information */
{
	int x, y, dummy;												/* X and Y and a dummy for crap */
	XQueryPointer (display, win, &win, &win,						/* Return results of pointer */
		&x, &y, &dummy, &dummy, &dummy);
	printf ("Pointer coordinates\n");								/* Header */
	printf ("X Position: %d\n", x);									/* X Coords */
	printf ("Y Position: %d\n", y);									/* Y Coords */
	return;															/* Exit function */
}
void showWindow(struct tag *pS)										/* Just show window information */
{
	char *tmpName;													/* Tempory name before copied to pS->windowName */
	XFetchName(display, win, &tmpName);								/* Fetch the tempory name */
    XWindowAttributes winAttribs;									/* define struct */
	if ( XGetWindowAttributes(display,win,&winAttribs) == 0) {
		if ( !pS->quietSet ) {										/* If -quiet called use int to display error */
			printf("%s: failed to get window attributes\n",pS->programName);
		}
		exit(1);													/* Break out of program */
	}
	if ( tmpName ) {												/* If a name has been set */
		strcpy(pS->windowName,tmpName);								/* Copy temp name to perm name */
	} else {														/* Else set name */
		strcpy(pS->windowName, "Root");								/* pS->windowname to Root window */
	}
	pS->windowX 		= winAttribs.x;								/* Store the X value */
	pS->windowY 		= winAttribs.y;								/* Store the Y value */
	pS->windowW 		= winAttribs.width;							/* Store the Width value */
	pS->windowH 		= winAttribs.height;						/* Store the Height value */
	pS->windowBorder	= winAttribs.border_width;					/* Store the border width */
	printf("Window Name:   %s\n",pS->windowName);					/* Print out the window name */
	printf("Window Id:     %s\n", pS->windowId);					/* Print the ID chosen */
	printf("geometry:      %dx%d+%d+%d\n",							/* Print the 'geometry */
		pS->windowW,
		pS->windowH,
		pS->windowX,
		pS->windowY);
	printf("X Position:    %d\n",pS->windowX);						/* Print out the X value */
	printf("Y Position:    %d\n",pS->windowY);						/* Print out the Y value */
	printf("Window Width:  %d\n",pS->windowW);						/* Print out the width */
	printf("Window Height: %d\n",pS->windowH);						/* Print out the height */
	printf("Broder Width:  %d\n",pS->windowBorder);					/* Print out the height */
	return;															/* return to main function */
}
void setPointer(struct tag *pS)										/* Set the pointer location */
{
	Display *display2;												/* Setup diferent dpy for pointer func */
	display2 = XOpenDisplay (NULL);									/* Init dpy */
	if ( pS->windowHidden ) {										/* If must set pointer to hidden */
		Pixmap pix;													/* The cursor pixmap */
		XColor colour;												/* The colour for the cursor */
		Cursor blank;												/* The blank cursor */
		pix = XCreatePixmap(
			display2,
			DefaultRootWindow(display2),
			1,
			1,
			1);
		colour.pixel = 0;
		colour.red = 0;
		colour.green = 0;
		colour.blue = 0;
		colour.flags = 0;
		colour.pad = 0;
		blank = XCreatePixmapCursor(
			display2,
			pix,
			pix,
			&colour,
			&colour,
			1,
			1);
		XDefineCursor(display2,
			DefaultRootWindow(display2),
			blank );
		XFlush(display2);
		XCloseDisplay (display2);									/* Close pointer dpy */
		return;
	}
	if ( pS->geometrySet ) {										/* If geometry needs to be set */
		int x, y, w, h, g;											/* Set some ints for return of XWMGeometry */
		win = 0;													/* Blank win */
		win = DefaultRootWindow(display2);							/* Set win for pointer dpy */
		XSizeHints hints = {0};										/* Set some hint defaults */
		XWMGeometry(display2, pS->displayScreen,					/* Get info from -geometry */
			pS->geometry, "100x100+0+0",
			0, &hints, &x, &y, &w, &h, &g);
		pS->windowX = x;											/* Copy new X to struct */
		pS->windowY = y;											/* Copy new Y to struct */
		pS->windowW = w;											/* Copy new W to struct */
		pS->windowH = h;											/* Copy new H to struct */
		XWarpPointer(display2, win, win,							/* Set pointer position */
			0, 0, 0, 0, x, y);
		XCloseDisplay (display2);									/* Close pointer dpy */
	}
}
void setWindow(struct tag *pS)										/* Set the window information */
{
	if ( pS->windowTop ) {											/* If must set window on top */
		XMapWindow(display,win);									/* First make sure the window is mapped */
		XRaiseWindow(display,win);									/* Raise the window to the top */
		return;
	}
	if ( pS->windowHidden ) {										/* If must set window to hidden */
		XUnmapWindow(display,win);									/* Hide the window */
		return;
	}
	if ( pS->geometrySet ) {										/* If geometry needs to be set */
		int x, y, w, h, g;											/* Set some ints for return of XWMGeometry */
		XSizeHints hints = {0};										/* Set some hint defaults */
		if (strcmp(pS->geometry, "fullscreen")) {					/* If -fullscreen wasn't entered */
			XWMGeometry(display, pS->displayScreen,					/* Get info from -geometry */
						pS->geometry, "100x100+0+0",
						0, &hints, &x, &y, &w, &h, &g);
		} else {													/* No -fullscreen */
			XWindowAttributes rootWinAttribs;						/* Define root struct */
			if ( XGetWindowAttributes(display,						/* Get info from root window */
					RootWindow(display,pS->displayScreen),
					&rootWinAttribs) == 0) {						/* There was an error */
				if ( !pS->quietSet ) {								/* If -quiet called use int to display error */
					printf("%s: failed to get window attributes\n",pS->programName);
				}
				exit(1);											/* Break out of program */
			}
			x 	= rootWinAttribs.x;									/* Store the X value */
			y 	= rootWinAttribs.y;									/* Store the Y value */
			w 	= rootWinAttribs.width;								/* Store the Width value */
			h 	= rootWinAttribs.height;							/* Store the Height value */
		}
		pS->windowX = x;											/* Copy new X to struct */
		pS->windowY = y;											/* Copy new Y to struct */
		pS->windowW = w;											/* Copy new W to struct */
		pS->windowH = h;											/* Copy new H to struct */
		XMoveResizeWindow(display, win,								/* Resize and move window */
			pS->windowX,
			pS->windowY,
			pS->windowW,
			pS->windowH);
	}
	XSetWindowBorderWidth(display, win,								/* Set the border width */
		pS->windowBorder);
	return;															/* Return to Main function */
}
void setWindowEase(struct tag *pS)									/* Set the window with easing */
{
	int x, y, w, h, g;												/* Set some ints for return of XWMGeometry */
	XSizeHints hints = {0};											/* Set some hint defaults */
    XWindowAttributes winAttribs;									/* define struct */
	if ( XGetWindowAttributes(display,win,&winAttribs) == 0) {
		if ( !pS->quietSet ) {										/* If -quiet called use int to display error */
			printf("%s: failed to get window attributes\n",pS->programName);
		}
		exit(1);													/* Break out of program */
	}
	pS->windowXInit		= winAttribs.x;								/* Store the X value */
	pS->windowYInit		= winAttribs.y;								/* Store the Y value */
	pS->windowWInit		= winAttribs.width;							/* Store the Width value */
	pS->windowHInit		= winAttribs.height;						/* Store the Height value */

	if (strcmp(pS->geometry, "fullscreen")) {						/* If -fullscreen wasn't entered */
		XWMGeometry(display, pS->displayScreen,						/* Get info from -geometry */
					pS->geometry, "100x100+0+0",
					0, &hints, &x, &y, &w, &h, &g);
	} else {														/* No -fullscreen */
		XWindowAttributes rootWinAttribs;							/* Define root struct */
		if ( XGetWindowAttributes(display,							/* Get info from root window */
				RootWindow(display,pS->displayScreen),
				&rootWinAttribs) == 0) {							/* There was an error */
			if ( !pS->quietSet ) {									/* If -quiet called use int to display error */
				printf("%s: failed to get window attributes\n",pS->programName);
			}
			exit(1);												/* Break out of program */
		}
		x 	= rootWinAttribs.x;										/* Store the X value */
		y 	= rootWinAttribs.y;										/* Store the Y value */
		w 	= rootWinAttribs.width;									/* Store the Width value */
		h 	= rootWinAttribs.height;								/* Store the Height value */
	}
	pS->windowX = x;												/* Copy new X to struct */
	pS->windowY = y;												/* Copy new Y to struct */
	pS->windowW = w;												/* Copy new W to struct */
	pS->windowH = h;												/* Copy new H to struct */
	pS->windowXDif = pS->windowX - pS->windowXInit;					/* Work out the difference for X */
	pS->windowYDif = pS->windowY - pS->windowYInit;					/* Work out the difference for Y */
	pS->windowWDif = pS->windowW - pS->windowWInit;					/* Work out the difference for W */
	pS->windowHDif = pS->windowH - pS->windowHInit;					/* Work out the difference for h */
	int tmpX = 0;													/* Create a temp X for ease */
	int tmpY = 0;													/* Create a temp y for ease */
	unsigned int tmpW = 200;										/* Create a temp W for ease */
	unsigned int tmpH = 200;										/* Create a temp H for ease */
	long int sleeptime = 1;											/* Set the sleep time for a cycle */
	while (pS->cycTime <= pS->duration) {							/* While the time is less than the duration */
		Display *display2;											/* Setup diferent dpy for pointer func */
		display2 = XOpenDisplay (NULL);								/* Init dpy */
		if (!strcmp(pS->ease, "io")) {								/* If using ease in/out */
			strcpy(pS->easeD,"x");
			tmpX = easeIo(pS);
			strcpy(pS->easeD,"y");
			tmpY = easeIo(pS);
			strcpy(pS->easeD,"w");
			tmpW = easeIo(pS);
			strcpy(pS->easeD,"h");
			tmpH = easeIo(pS);
		}
		if (!strcmp(pS->ease, "oi")) {								/* If using ease in/out */
			strcpy(pS->easeD,"x");
			tmpX = easeOi(pS);
			strcpy(pS->easeD,"y");
			tmpY = easeOi(pS);
			strcpy(pS->easeD,"w");
			tmpW = easeOi(pS);
			strcpy(pS->easeD,"h");
			tmpH = easeOi(pS);
		}
		if (!strcmp(pS->ease, "in")) {								/* If using ease in */
			strcpy(pS->easeD,"x");
			tmpX = easeIn(pS);
			strcpy(pS->easeD,"y");
			tmpY = easeIn(pS);
			strcpy(pS->easeD,"w");
			tmpW = easeIn(pS);
			strcpy(pS->easeD,"h");
			tmpH = easeIn(pS);
		}
		if (!strcmp(pS->ease, "out")) {								/* If using ease out */
			strcpy(pS->easeD,"x");
			tmpX = easeOut(pS);
			strcpy(pS->easeD,"y");
			tmpY = easeOut(pS);
			strcpy(pS->easeD,"w");
			tmpW = easeOut(pS);
			strcpy(pS->easeD,"h");
			tmpH = easeOut(pS);
		}
		XMoveResizeWindow(display2, win,							/* Resize and move window */
			tmpX,
			tmpY,
			tmpW,
			tmpH);
		pS->cycTime++;												/* Increase each cycle */
		usleep ((unsigned long int) sleeptime * 1000);
		XCloseDisplay (display2);									/* Close pointer dpy */
	}
	return;															/* return to main */
}
int easeIn(struct tag *pS)											/* Ease in Function */
{
	float tmpTime = pS->cycTime;									/* Create local time float */
	int tmpDif = 0;													/* Create local source/destination dif */
	int tmpInit = 0;
	if (!strcmp(pS->easeD, "x")) {
		tmpDif = pS->windowXDif;
		tmpInit = pS->windowXInit;
	}
	if (!strcmp(pS->easeD, "y")) {
		tmpDif = pS->windowYDif;
		tmpInit = pS->windowYInit;
	}
	if (!strcmp(pS->easeD, "w")) {
		tmpDif = pS->windowWDif;
		tmpInit = pS->windowWInit;
	}
	if (!strcmp(pS->easeD, "h")) {
		tmpDif = pS->windowHDif;
		tmpInit = pS->windowHInit;
	}
	tmpTime /= pS->duration;	
	return tmpDif*tmpTime*tmpTime*tmpTime + tmpInit;
}
int easeOut(struct tag *pS)											/* Ease out Function */
{
	float tmpTime = pS->cycTime;
	int tmpDif = 0;
	int tmpInit = 0;
	if (!strcmp(pS->easeD, "x")) {
		tmpDif = pS->windowXDif;
		tmpInit = pS->windowXInit;
	}
	if (!strcmp(pS->easeD, "y")) {
		tmpDif = pS->windowYDif;
		tmpInit = pS->windowYInit;
	}
	if (!strcmp(pS->easeD, "w")) {
		tmpDif = pS->windowWDif;
		tmpInit = pS->windowWInit;
	}
	if (!strcmp(pS->easeD, "h")) {
		tmpDif = pS->windowHDif;
		tmpInit = pS->windowHInit;
	}
	tmpTime /= pS->duration;
	tmpTime--;
	return tmpDif*(tmpTime*tmpTime*tmpTime + 1) + tmpInit;
}
int easeIo(struct tag *pS)											/* Ease in/out Function */
{
	float tmpTime = pS->cycTime;
	int tmpDif = 0;
	int tmpInit = 0;
	if (!strcmp(pS->easeD, "x")) {
		tmpDif = pS->windowXDif;
		tmpInit = pS->windowXInit;
	}
	if (!strcmp(pS->easeD, "y")) {
		tmpDif = pS->windowYDif;
		tmpInit = pS->windowYInit;
	}
	if (!strcmp(pS->easeD, "w")) {
		tmpDif = pS->windowWDif;
		tmpInit = pS->windowWInit;
	}
	if (!strcmp(pS->easeD, "h")) {
		tmpDif = pS->windowHDif;
		tmpInit = pS->windowHInit;
	}
	tmpTime /= pS->duration/2;
	if (tmpTime < 1) {
		return tmpDif/2*tmpTime*tmpTime*tmpTime + tmpInit;
	}
	tmpTime -= 2;
	return tmpDif/2*(tmpTime*tmpTime*tmpTime + 2) + tmpInit;
}
int easeOi(struct tag *pS)											/* Ease out/in Function */
{
	float tmpTime = pS->cycTime;									/* Create local time float */
	int tmpDif = 0;													/* Create local source/destination dif */
	int tmpInit = 0;
	if (!strcmp(pS->easeD, "x")) {
		tmpDif = pS->windowXDif;
		tmpInit = pS->windowXInit;
	}
	if (!strcmp(pS->easeD, "y")) {
		tmpDif = pS->windowYDif;
		tmpInit = pS->windowYInit;
	}
	if (!strcmp(pS->easeD, "w")) {
		tmpDif = pS->windowWDif;
		tmpInit = pS->windowWInit;
	}
	if (!strcmp(pS->easeD, "h")) {
		tmpDif = pS->windowHDif;
		tmpInit = pS->windowHInit;
	}
	float ts=(tmpTime/=pS->duration)*tmpTime;
	float tc=ts*tmpTime;
	return tmpInit+tmpDif*(4*tc + -6*ts + 3*tmpTime);
}
/* vim: set ts=4 sw=4 nowrap: */
