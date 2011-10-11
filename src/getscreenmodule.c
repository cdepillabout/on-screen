
#include <Python.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>


static int _XlibErrorHandler(Display *display, XErrorEvent *event) {
	fprintf(stderr, "An error occured detecting the mouse position\n");
	return True;
}

/* Return what screen the pointer is currently on. */
static int get_xinerama_screen(int debug) {
	int screen;
	int i;
    int pointer_screen_res = -1;
	Bool result;
	Window root_window;
	Window window_returned;

	XineramaScreenInfo *xinerama_screens = NULL;
	int num_xinerama_screens = -1;

	int root_x, root_y;
	int win_x, win_y;
	unsigned int mask_return;

	// get the display
	Display *display = XOpenDisplay(NULL);
	assert(display);
	XSetErrorHandler(_XlibErrorHandler);
	screen = XDefaultScreen(display);
	root_window = XRootWindow(display, screen);

	// get the xinerama screens
	xinerama_screens = XineramaQueryScreens(display, &num_xinerama_screens);
	if (!xinerama_screens || num_xinerama_screens <= 0) {
		fprintf(stderr, "ERROR! Could not find any Xinerama screens.\n");
        pointer_screen_res = -1;
        goto out;
	}
	if (debug) {
		fprintf(stderr, "There are %d xinerama screens available in this X session\n",
				num_xinerama_screens);
		for (i = 0; i < num_xinerama_screens; i++) {
			fprintf(stderr, "\tXinerama Screen %d, %hdx%hd+%hd+%hd\n",
					xinerama_screens[i].screen_number,
					xinerama_screens[i].width,
					xinerama_screens[i].height,
					xinerama_screens[i].x_org,
					xinerama_screens[i].y_org);
		}
	}

	// find where the mouse is
	result = XQueryPointer(display, root_window, &window_returned,
			&window_returned, &root_x, &root_y, &win_x, &win_y,
			&mask_return);
	if (result != True) {
		fprintf(stderr, "ERROR! No mouse found.\n");
        pointer_screen_res = -1;
        goto out;
	}
	if (debug) {
		fprintf(stderr, "Mouse is at (%d,%d)\n", root_x, root_y);
	}

	for (i = 0; i < num_xinerama_screens; i++) {
		short left_x = xinerama_screens[i].x_org;
		short right_x = xinerama_screens[i].x_org + xinerama_screens[i].width;
		short top_y = xinerama_screens[i].y_org;
		short bottom_y = xinerama_screens[i].y_org + xinerama_screens[i].height;

		if (root_x >= left_x && root_x <= right_x && root_y >= top_y && root_y <= bottom_y) {
			// we have a winner
			//fprintf(stdout, "Xinerama Screen %d\n", xinerama_screens[i].screen_number);
			//fprintf(stdout, "%d\n", xinerama_screens[i].screen_number);
            pointer_screen_res = xinerama_screens[i].screen_number;
			break;
		}
	}

out:
	XFree(xinerama_screens);
	return pointer_screen_res;
}

/*
void usage(char *program_name) {
	fprintf(stdout, "usage: %s [-h] [-d]\n", program_name);
	fprintf(stdout, "Get the current Xinerama screen that the cursor is on.\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "optional arguments:\n");
	fprintf(stdout, " -h   show this help\n");
	fprintf(stdout, " -d   print debugging information to stderr\n");
}
*/

/*
int main(int argc, char **argv) {

	// this getopt code is from here
	// http://www.gnu.org/s/hello/manual/libc/Example-of-Getopt.html
	int debug_flag = 0;
	int c;

	opterr = 0;

	while ((c = getopt (argc, argv, "hd")) != -1) {
		switch (c)
		{
			case 'h':
				usage(argv[0]);
				return 0;
			case 'd':
				debug_flag = 1;
				break;
			case '?':
				if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
			default:
				abort ();
		}
	}

	return get_xinerama_screen(debug_flag);

}
*/


static PyObject *getscreen_getscreen(PyObject *self, PyObject *args)
{
    return Py_BuildValue("i", get_xinerama_screen(0));
}

static PyMethodDef GetScreenMethods[] = {
      {"getscreen", getscreen_getscreen, METH_VARARGS,
       "Get the current Xinerama screen the mouse is on."},
      {NULL, NULL, 0, NULL}
};

static struct PyModuleDef getscreen = {
    PyModuleDef_HEAD_INIT,
    "getscreen",            /* name of module */
    NULL,                   /* module documentation, may be NULL */
    -1,                     /* size of per-interpreter state of the module,
                               or -1 if the module keeps state in global variables. */
    GetScreenMethods
};

PyMODINIT_FUNC PyInit_getscreen(void)
{
    return PyModule_Create(&getscreen);
}
