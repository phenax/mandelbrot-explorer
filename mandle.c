#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>

#include<X11/Xlib.h>
#include <X11/Xutil.h>

#include "complex.h"

Display *dpy;
int screen = 0;
Window root, win;
unsigned int width, height;

int offset_x = 0, offset_y = 0;
double scale_offset = 0;

typedef struct Step { unsigned int step; XColor color; GC gc; } Step;

#define GRADIENT_STEPS 6
Step steps[GRADIENT_STEPS];

XColor to_xcolor(const char *colorstr) {
  XColor ptr, dummy;
  XAllocNamedColor(dpy, DefaultColormap(dpy, screen), colorstr, &ptr, &dummy);
  return ptr;
}

GC color_gc(XColor c) {
  XGCValues gr_values;
  gr_values.foreground = c.pixel;
  GC gc = XCreateGC(dpy, win, GCForeground, &gr_values);
  return gc;
}

void initialize_colors() {
  int i = 0;
  steps[i++] = (Step) { 0,    to_xcolor("#000557"), NULL };
  steps[i++] = (Step) { 8,    to_xcolor("#101567"), NULL };
  steps[i++] = (Step) { 20,   to_xcolor("#5055a7"), NULL };
  steps[i++] = (Step) { 50,   to_xcolor("#ffffff"), NULL };
  steps[i++] = (Step) { 100,  to_xcolor("#ffffff"), NULL };
  steps[i++] = (Step) { 200,  to_xcolor("#000000"), NULL };
}

void plot_mandlebrot() {
  ComplexNumber c = { 0, 0 };
  ComplexNumber z = { 0, 0 };
  int i, j, s, count;
  double mag, prev_mag;

  double threshold = 1000;
  double size = width;
  int max_iterations = steps[GRADIENT_STEPS - 1].step;

  for(int i = 0; i < GRADIENT_STEPS; i++) {
    steps[i].gc = color_gc(steps[i].color);
  }

  double scale = 4 + scale_offset;

  XClearWindow(dpy, win);

  for(i = 0; i < size; i++) {
    c.real = (offset_x + (double) i - (width / 2)) * scale / size;

    for (j = 0; j < size; j++) {
      c.im = (offset_y + (double) j - (height / 2)) * scale / size;

      z.real = z.im = 0;
      mag = magnitude(z);
      prev_mag = magnitude(z);

      for(count = 0; absf(mag - prev_mag) < threshold && count < max_iterations; count++) {
        z = add(square(z), c);
        prev_mag = mag;
        mag = magnitude(z);
      }

      for (s = 0; s < GRADIENT_STEPS; s++) {
        if (count >= steps[s].step) {
          XDrawPoint(dpy, win, steps[s].gc, i, j);
        }
      }
    }
  }

  XSync(dpy, False);
}

void keypress(XKeyEvent ev) {
  KeySym keysym = XKeycodeToKeysym(dpy, (KeyCode) ev.keycode, 0);
  int movement = 10;
  double zoomdiff = 0.3;

  char should_rerender = True;

  switch(keysym) {
    case XK_h:
      offset_x += movement;
      break;
    case XK_l:
      offset_x -= movement;
      break;
    case XK_j:
      offset_y += movement;
      break;
    case XK_k:
      offset_y -= movement;
      break;
    case XK_equal:
      scale_offset -= zoomdiff;
      break;
    case XK_minus:
      scale_offset += zoomdiff;
      break;
    case XK_space:
      offset_x = 0;
      offset_y = 0;
      scale_offset = 0;
      break;
    default: should_rerender = False;
  }

  if (should_rerender) {
    plot_mandlebrot();
  }
}

void run_event_loop() {
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
      case KeyRelease:
        keypress(ev.xkey);
        break;
    }
  }
}

int main() {
  dpy = XOpenDisplay(NULL);
  root = DefaultRootWindow(dpy);

  initialize_colors();

  win = XCreateSimpleWindow(dpy, root, 100, 100, 500, 500, 1, BlackPixel(dpy, screen), steps[0].color.pixel);

  XSelectInput(dpy, win, ExposureMask | KeyReleaseMask | StructureNotifyMask);
  XMapWindow(dpy, win);
  XSync(dpy, False);

  run_event_loop();

  XCloseDisplay(dpy);
}

