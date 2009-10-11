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

/* Stop multiple inclusions */
#ifndef _HAVE_XLAYOUT_
#define _HAVE_XLAYOUT_

#include <config.h>

#ifdef HAVE_XLAYOUT
#include <ease.h>
#endif

/* Enumeration for toggling state */
enum toggle {UNSET=0, SET=1};

/* Display information */
typedef struct {
	Display *display;
	char *name;
	int screen;
	int screen_count;
} XLDisplay;

/* Pointer information */
typedef struct {
	int x;
	int y;
	char *new_geometry;
	enum toggle visible;
	XColor colour;
	Cursor blank;
	Pixmap pix;
} XLPointer;

/* Window Information */
typedef struct {
	Window window;
	char *name;
	char *id;
	/* Window geometry */
	int x;
	int y;
	unsigned int w;
	unsigned int h;
	unsigned int border;
	/* Window visibility */
	enum toggle visible;
	enum toggle top;
	/* New window settings */
	char *new_geometry;
	unsigned int new_border;
} XLWindow;

/* Utility functions */
void debug(short int level, char *msg, ...);
void display_version();
void display_help();

/* Display functions */
int create_display();
int destroy_display();
int count_screens();

/* Pointer Functions */
void display_pointer();
void hide_pointer();
void set_pointer();
void get_pointer(XLPointer *gp);
#ifdef _HAVE_LIBEASE_
int ease_pointer(Ease_Multi *e, va_list ap);
#endif

/* Window functions */
void list_windows(Window w, int depth);
void create_window(XLWindow *w);
Window create_window_named(char *name, Window root);
void display_window(XLWindow *w);
void show_window(XLWindow *w);
void hide_window(XLWindow *w);
void top_window(XLWindow *w);
void window_border(XLWindow *w);
void set_window(XLWindow *w);
void get_window(XLWindow *gw);
#ifdef _HAVE_LIBEASE_
int ease_window(Ease_Multi *e, va_list ap);
#endif

#endif
