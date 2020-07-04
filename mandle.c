#include<stdio.h>
#include<stdlib.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>

#include "complex.h"

// TODO: Multithread the drawing

typedef struct Step { unsigned int step; XColor color; GC gc; char should_paint } Step;

Display *dpy;
int screen = 0;
Window root, win;
unsigned int width, height;

double offset_x = 0, offset_y = 0;
double scale_offset = 1;

Step steps[10];
unsigned int step_count = 0;

XColor background;

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
  background = to_xcolor("#0f0c19");
  steps[step_count++] = (Step) { 200,   to_xcolor("#000000"), NULL, 0 };
  steps[step_count++] = (Step) { 100,   to_xcolor("#ffffff"), NULL, 1 };
  steps[step_count++] = (Step) { 30,    to_xcolor("#4e3aA3"), NULL, 1 };
}

void plot_mandlebrot() {
  ComplexNumber c = { 0, 0 };
  ComplexNumber z = { 0, 0 };
  int i, j, s, count;
  double mag, prev_mag;

  double threshold = 1000;
  double size = width;
  int max_iterations = steps[0].step;

  for(s = 0; s < step_count; s++) {
    if (steps[s].should_paint) {
      steps[s].gc = color_gc(steps[s].color);
    }
  }

  double scale = 4 * absf(scale_offset);

  XClearWindow(dpy, win);

  for(i = 0; i < size; i++) {
    c.real = offset_x + ((double) i - (width / 2)) * scale / size;

    for (j = 0; j < size; j++) {
      c.im = offset_y + ((double) j - (height / 2)) * scale / size;

      z.real = z.im = 0;
      mag = magnitude(z);
      prev_mag = mag;

      for(count = 0; absf(mag - prev_mag) < threshold && count < max_iterations; count++) {
        z = add(square(z), c);
        prev_mag = mag;
        mag = magnitude(z);
      }

      for (s = 0; s < step_count; s++) {
        if (steps[s].step != 0 && count >= steps[s].step) {
          if (steps[s].gc)
            XDrawPoint(dpy, win, steps[s].gc, i, j);
          break;
        }
      }
    }
  }

  XSync(dpy, False);
}

int quantifier = 0;
void keypress(XKeyEvent ev) {
  int tmp;
  KeySym *keysym = XGetKeyboardMapping(dpy, ev.keycode, 1, &tmp);

  double movement = 0.2;
  double zoomdiff = 1.2;

  char should_rerender = True;

  if (*keysym >= XK_0 && *keysym <= XK_9) {
    quantifier = (quantifier * 10) + (*keysym ^ XK_0);
    return;
  }

  int times = quantifier ? quantifier : 1;
  switch(*keysym) {
    // Absolute movement
    case XK_h: offset_x -= movement * times; break;
    case XK_l: offset_x += movement * times; break;
    case XK_j: offset_y += movement * times; break;
    case XK_k: offset_y -= movement * times; break;

    // Slow movement
    case XK_Left: offset_x -= scale_offset * movement * times; break;
    case XK_Right: offset_x += scale_offset * movement * times; break;
    case XK_Up: offset_y -= scale_offset * movement * times; break;
    case XK_Down: offset_y += scale_offset * movement * times; break;

    // Zoom
    case XK_equal: scale_offset /= zoomdiff*times; break;
    case XK_minus: scale_offset *= zoomdiff*times; break;

    case XK_space: // Reset
      offset_x = 0;
      offset_y = 0;
      scale_offset = 1;
      break;
    default: should_rerender = False;
  }

  quantifier = 0;

  if (should_rerender) {
    plot_mandlebrot();
  }

  XFree(keysym);
}

void cleanup() {
  if(dpy) XCloseDisplay(dpy);
  if(win) XDestroyWindow(dpy, win);
}

int error_handler() {
  cleanup();
  return 0;
}

void run_event_loop() {
  XSetErrorHandler(error_handler);
  XEvent ev;
  XConfigureEvent cev;

  Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(dpy, win, &wmDeleteMessage, 1);

  int running = True;
  char should_rerender = True;

  while(running) {
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
      case ClientMessage:
        if (ev.xclient.data.l[0] == wmDeleteMessage) {
          cleanup();
          running = False;
        }
        break;
    }
  }

  cleanup();
}

int main() {
  dpy = XOpenDisplay(NULL);
  root = DefaultRootWindow(dpy);

  initialize_colors();

  win = XCreateSimpleWindow(dpy, root, 100, 100, 500, 500, 1, BlackPixel(dpy, screen), background.pixel);

  XSelectInput(dpy, win, ExposureMask | KeyReleaseMask | StructureNotifyMask);
  XMapWindow(dpy, win);
  XSync(dpy, False);

  run_event_loop();

  XCloseDisplay(dpy);
}

