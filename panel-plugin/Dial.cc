#include <glib-object.h>
#include <gtk/gtk.h>
#include <librsvg/rsvg.h>

#include <cmath>

#define DEFAULT_WIDTH 256
#define DEFAULT_HEIGHT 256
#define PI 3.1415926535

typedef struct color_t {
  double alpha;
  double red;
  double green;
  double blue;

  color_t(unsigned color)
      : alpha(((color & 0xff000000) >> 24) / 255.0),
        red(((color & 0x00ff0000) >> 16) / 255.0),
        green(((color & 0x0000ff00) >> 8) / 255.0),
        blue((color & 0x000000ff) / 255.0) {
    ;
  }
} color_t;

typedef struct point_t {
  double x;
  double y;
} point_t;

typedef struct dial_t {
  // The degrees across which the dial will sweep
  const double sweep;

  // The angle at which the sweep begins. This is the "conventional" angle in
  // old-fashioned coordinate geometry, where the angle is measured
  // anti-clockwise from the positive X-axis which increases to the east.
  const double sweep_begin;

  // The number of stops in the dial. This is 
  const unsigned stops;
  
  // The current position of dial. This will be in the range [0-100], inclusive
  unsigned current;
} dial_t;

// Colors in the gradients
static const color_t colors[] = {
    0xff1d9e27, 0xff1daf28, 0xff38bb2f, 0xff32cd33, 0xff42d030, 0xff50d32f,
    0xff61d52c, 0xff74d82a, 0xff86db26, 0xff99dd24, 0xffade021, 0xffc4e220,
    0xffdae51d, 0xffe7dd1b, 0xfff9e000, 0xffffe001, 0xfffec901, 0xfffeb415,
    0xffff9e13, 0xffff870f, 0xfff66e0b, 0xfff75409, 0xfffb3906, 0xfffd1d03,
    0xfffe0000, 0xfff60607, 0xffed0d0d, 0xffd71112, 0xffc50500, 0xffb40001};
static constexpr unsigned stops = sizeof(colors) / sizeof(color_t);

// The range of input to the dial
static constexpr range_t range = {0, 100};

// The degrees across which the dial will sweep
static constexpr double sweep = 180;

static constexpr double sweep_begin = 0;

// The degrees through which each gradient steps
static constexpr double step = sweep / stops;

static double radians(double degrees) { return degrees * PI / 180; }

static void draw_dial(cairo_t *cr, unsigned width, unsigned height) {
  double radius, stroke, step;
  double dim;
  unsigned i;
  unsigned angle;

  dim = fmin(0.5 * width, 0.5 * height);
  stroke = 0.3333 * dim;
  radius = 0.8333 * dim; // radius = dim - stroke / 2
  cairo_set_line_width(cr, stroke);
  step = dial.sweep / dial.stops;
  // Cairo measures angles clockwise, but all my trigonometry and coordinate
  // geometry is done assuming the angles are measured anti-clockwise
  for (i = 0, angle = 360 - sweep_begin; i < stops; i++, angle += step) {
    cairo_set_source_rgb(cr, colors[i].red, colors[i].green, colors[i].blue);
    cairo_arc(cr, 0, 0, radius, radians(angle), radians(angle + step));
    cairo_stroke(cr);
  }
}

static void draw_knob(cairo_t *cr, unsigned width, unsigned height) {
  double dim;
  double radius, stroke;

  dim = fmin(0.5 * width, 0.5 * height);
  radius = fmax(0.0695 * dim, 1.5);
  stroke = fmax(0.0521 * dim, 1.0);

  cairo_set_line_width(cr, stroke);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_arc(cr, 0, 0, radius, radians(0), radians(360));
  cairo_stroke(cr);
}

static void draw_hand(cairo_t *cr, unsigned width, unsigned height,
                      int magnitude) {
  double dim;
  double radius;
  double theta, thetap, thetam;
  point_t p, b1, b2;
  const double delta = 6.5;

  dim = fmin(0.5 * width, 0.5 * height);
  radius = 0.6667 * dim;
  theta = sweep_begin - ((sweep / (range.max - range.min)) * magnitude);
  thetap = theta + delta;
  thetam = theta - delta;

  // In the "classical" coordinate system, the positive Y-axis increases to
  // the north, whereas here, it increases to the south. So we need to invert
  // the sign of the Y-coordinate
  p = {radius * cos(radians(theta)), -radius * sin(radians(theta))};

  // Coordinates of the base points that are tangents to the central knob
  // are offset from the apex of the pointer. We need this so we can close the
  // path correctly and fill it
  b1 = {p.x - radius * cos(radians(thetap)),
        p.y + radius * sin(radians(thetap))};
  b2 = {p.x - radius * cos(radians(thetam)),
        p.y + radius * sin(radians(thetam))};

  cairo_set_line_width(cr, 1);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);

  cairo_move_to(cr, p.x, p.y);
  cairo_line_to(cr, b1.x, b1.y);
  cairo_line_to(cr, b2.x, b2.y);
  cairo_close_path(cr);
  cairo_fill(cr);
}

static void draw(cairo_t *cr, unsigned width, unsigned height, int magnitude) {
  // Move the axes to the center of the canvas so all the calculations can be
  // done with the "conventional" coordinate system
  cairo_translate(cr, 0.5 * width, 0.5 * height);
  draw_dial(cr, width, height);
  draw_knob(cr, width, height);
  draw_hand(cr, width, height, magnitude);
}

static gboolean cb_draw(GtkWidget *canvas, cairo_t *cr, gpointer data) {
  GtkAllocation allocation;
  status_t *status;

  status = (status_t *)data;
  gtk_widget_get_allocation(canvas, &allocation);
  draw(cr, allocation.width, allocation.height, status->magnitude);

  return FALSE;
}

gboolean tick(gpointer data) {
  status_t *status = (status_t *)data;

  if (status->magnitude == range.min) {
    if (not status->up)
      status->up = true;
  } else if (status->magnitude == range.max) {
    if (status->up)
      status->up = false;
  }
  if (status->up)
    status->magnitude += 1;
  else
    status->magnitude -= 1;

  gtk_widget_queue_draw(status->canvas);

  return TRUE;
}

int main(int argc, char *argv[]) {
  GtkWidget *window, *canvas;
  unsigned width, height, magnitude;
  status_t *status;

  width = DEFAULT_WIDTH;
  height = DEFAULT_HEIGHT;
  magnitude = 0;
  if (argc > 1)
    width = atoi(argv[1]);
  if (argc > 2)
    height = atoi(argv[2]);
  if (argc > 3)
    magnitude = atoi(argv[3]);

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(G_OBJECT(window), "destroy", gtk_main_quit, NULL);
  g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);

  canvas = gtk_drawing_area_new();
  gtk_widget_set_size_request(canvas, width, height);
  status = g_new(status_t, 1);
  status->canvas = canvas;
  status->magnitude = magnitude;
  status->up = true;
  g_signal_connect(G_OBJECT(canvas), "draw", G_CALLBACK(cb_draw), status);

  g_timeout_add(50, tick, status);

  gtk_container_add(GTK_CONTAINER(window), canvas);
  gtk_widget_show_all(window);

  gtk_main();

  // Cleanup here
  g_free(status);

  return 0;
}
