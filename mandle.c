#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>

#include<X11/Xlib.h>

#include "complex.h"

Display *dpy;
int screen = 0;
Window root, win;
unsigned int width, height;

XColor to_xcolor(const char *colorstr) {
  XColor ptr, dummy;
  XAllocNamedColor(dpy, DefaultColormap(dpy, screen), colorstr, &ptr, &dummy);
  return ptr;
}

GC color_gc(char* color) {
  XGCValues gr_values;
  gr_values.foreground = to_xcolor(color).pixel;
  GC gc = XCreateGC(dpy, win, GCForeground, &gr_values);
  return gc;
}

int max_iterations = 200;

void plot_mandlebrot() {
  ComplexNumber c = { 0, 0 };
  ComplexNumber z = { 0, 0 };
  int i, j;
  int count = 0;
  double mag, prev_mag;

  GC blue_gc = color_gc("#0000ff");
  GC red_gc = color_gc("#ff5555");
  GC white_gc = color_gc("#ffffff");

  double threshold = 10000;

  double scale = 4;
  double size = width;

  XClearWindow(dpy, win);

  for(i = 0; i < size; i++) {
    c.real = ((double) i - (width / 2)) * scale / size;

    for (j = 0; j < size; j++) {
      c.im = ((double) j - (height / 2)) * scale / size;

      z.real = z.im = 0;
      mag = magnitude(z);
      prev_mag = magnitude(z);

      for(count = 0; absf(mag - prev_mag) < threshold && count < max_iterations; count++) {
        z = add(square(z), c);
        prev_mag = mag;
        mag = magnitude(z);
      }

      if (count == max_iterations) {
        XDrawPoint(dpy, win, red_gc, i, j);
      }
    }
  }

  XSync(dpy, False);
}

void run_event_loop() {
  int tmp;
  unsigned int utmp;

  XEvent ev;
  XConfigureEvent cev;

  char should_rerender = True;

  while(1) {
    XNextEvent(dpy, &ev);

    switch(ev.type) {
      case Expose:
        if (should_rerender) {
          plot_mandlebrot();
          should_rerender = False;
        }
        break;
      case ConfigureNotify:
        cev = ev.xconfigure;
        if (cev.width != width || cev.height != height) {
          width = cev.width;
          height = cev.height;
          should_rerender = True;
        }
        break;
    }
  }
}

int main() {
  dpy = XOpenDisplay(NULL);
  root = DefaultRootWindow(dpy);

  win = XCreateSimpleWindow(dpy, root, 0, 0, 100, 100, 1, BlackPixel(dpy, screen), BlackPixel(dpy, screen));

  XSelectInput(dpy, win, ExposureMask | StructureNotifyMask);

  XMapWindow(dpy, win);

  XSync(dpy, False);

  run_event_loop();

  XCloseDisplay(dpy);
}

