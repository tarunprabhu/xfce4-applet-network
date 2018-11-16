#include "Dial.h"

#include "Constants.h"

#include <cmath>

// Definitions of static data members
const std::vector<Color> Dial::ColorsRedToGreen = {
    0xffb40001, 0xffc50500, 0xffd71112, 0xffed0d0d, 0xfff60607, 0xfffe0000,
    0xfffd1d03, 0xfffb3906, 0xfff75409, 0xfff66e0b, 0xffff870f, 0xffff9e13,
    0xfffeb415, 0xfffec901, 0xffffe001, 0xfff9e000, 0xffe7dd1b, 0xffdae51d,
    0xffc4e220, 0xffade021, 0xff99dd24, 0xff86db26, 0xff74d82a, 0xff61d52c,
    0xff50d32f, 0xff42d030, 0xff32cd33, 0xff38bb2f, 0xff1daf28, 0xff1d9e27};

const std::vector<Color> Dial::ColorsGreenToRed = {
    0xff1d9e27, 0xff1daf28, 0xff38bb2f, 0xff32cd33, 0xff42d030, 0xff50d32f,
    0xff61d52c, 0xff74d82a, 0xff86db26, 0xff99dd24, 0xffade021, 0xffc4e220,
    0xffdae51d, 0xffe7dd1b, 0xfff9e000, 0xffffe001, 0xfffec901, 0xfffeb415,
    0xffff9e13, 0xffff870f, 0xfff66e0b, 0xfff75409, 0xfffb3906, 0xfffd1d03,
    0xfffe0000, 0xfff60607, 0xffed0d0d, 0xffd71112, 0xffc50500, 0xffb40001};

// Utilities
static double radians(double degrees) {
  return degrees * Constants::Pi / 180;
}

// Callbacks
static gboolean cb_timer_tick(gpointer data) {
  return reinterpret_cast<Dial*>(data)->cbTimerTick();
}

static gboolean cb_canvas_draw(GtkWidget* w, cairo_t* cr, gpointer data) {
  return reinterpret_cast<Dial*>(data)->cbCanvasDraw(GTK_DRAWING_AREA(w), cr);
}

Dial::Dial(GtkWidget*                canvas,
           double                    sweep,
           double                    sweep0,
           double                    minVal,
           double                    maxVal,
           double                    period,
           const std::vector<Color>& colors)
    : canvas(canvas), current(0), target(0), timer(0) {
  opts.width  = gtk_widget_get_allocated_width(canvas);
  opts.height = gtk_widget_get_allocated_height(canvas);
  opts.sweep  = sweep;
  opts.sweep0 = sweep0;
  opts.minVal = minVal;
  opts.maxVal = maxVal;
  opts.period = period;
  opts.colors = colors;
  g_message("(w, h) = (%d, %d)", opts.width, opts.height);
}

void Dial::drawDial(cairo_t* cr) {
  double dim    = fmin(0.5 * opts.width, 0.5 * opts.height);
  double stroke = 0.3333 * dim;
  double radius = 0.8333 * dim; // radius = dim - stroke / 2
  double step = opts.sweep / opts.colors.size();

  cairo_set_line_width(cr, stroke);
  // Cairo measures angles clockwise, but all my trigonometry and coordinate
  // geometry is done assuming the angles are measured anti-clockwise
  for(unsigned i = 0, angle = 360 - opts.sweep0; i < opts.colors.size();
      i++, angle += step) {
    cairo_set_source_rgb(cr, opts.colors[i].red, opts.colors[i].green,
                         opts.colors[i].blue);
    cairo_arc(cr, 0, 0, radius, radians(angle), radians(angle + step));
    cairo_stroke(cr);
  }
}

void Dial::drawKnob(cairo_t* cr) {
  double dim    = fmin(0.5 * opts.width, 0.5 * opts.height);
  double radius = fmax(0.0695 * dim, 1.5);
  double stroke = fmax(0.0521 * dim, 1.0);

  cairo_set_line_width(cr, stroke);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_arc(cr, 0, 0, radius, radians(0), radians(360));
  cairo_stroke(cr);
}

void Dial::drawHand(cairo_t* cr) {
  const double delta = 6.5;

  double dim    = fmin(0.5 * opts.width, 0.5 * opts.height);
  double radius = 0.6667 * dim;
  double theta =
      opts.sweep0 - ((opts.sweep / (opts.maxVal - opts.minVal)) * current);
  double thetap = theta + delta;
  double thetam = theta - delta;

  // In the "classical" coordinate system, the positive Y-axis increases to
  // the north, whereas here, it increases to the south. So we need to invert
  // the sign of the Y-coordinate
  Point p = {radius * cos(radians(theta)), -radius * sin(radians(theta))};

  // Coordinates of the base points that are tangents to the central knob
  // are offset from the apex of the pointer. We need this so we can close the
  // path correctly and fill it
  Point b1 = {p.x - radius * cos(radians(thetap)),
              p.y + radius * sin(radians(thetap))};
  Point b2 = {p.x - radius * cos(radians(thetam)),
              p.y + radius * sin(radians(thetam))};

  cairo_set_line_width(cr, 1);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);

  cairo_move_to(cr, p.x, p.y);
  cairo_line_to(cr, b1.x, b1.y);
  cairo_line_to(cr, b2.x, b2.y);
  cairo_close_path(cr);
  cairo_fill(cr);
}

void Dial::draw(cairo_t* cr) {
  // Move the axes to the center of the canvas so all the calculations can be
  // done with the "conventional" coordinate system
  cairo_translate(cr, 0.5 * opts.width, 0.5 * opts.height);
  drawDial(cr);
  drawKnob(cr);
  drawHand(cr);
}

void Dial::setTarget(double newTarget) {
  target = newTarget;
  if(current > target)
    step = -1;
  else if(current < target)
    step = +1;
  else
    step = 0;

  if(step)
    if(timer == 0)
      timer = g_timeout_add(opts.period, cb_timer_tick, this);
}

void Dial::setPeriod(double newPeriod) {
  opts.period = newPeriod;
  if(timer) {
    g_source_remove(timer);
    timer = g_timeout_add(opts.period, cb_timer_tick, this);
  }
}

gboolean Dial::cbCanvasDraw(GtkDrawingArea*, cairo_t* cr) {
  draw(cr);
  return FALSE;
}

gboolean Dial::cbTimerTick() {
  current = current + step;
  gtk_widget_queue_draw(canvas);

  if(current == target)
    return FALSE;
  return TRUE;
}

// int main(int argc, char* argv[]) {
//   GtkWidget *window, *canvas;
//   unsigned   width, height, magnitude;
//   status_t*  status;

//   width     = DEFAULT_WIDTH;
//   height    = DEFAULT_HEIGHT;
//   magnitude = 0;
//   if(argc > 1)
//     width = atoi(argv[1]);
//   if(argc > 2)
//     height = atoi(argv[2]);
//   if(argc > 3)
//     magnitude = atoi(argv[3]);

//   gtk_init(&argc, &argv);

//   window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//   g_signal_connect(G_OBJECT(window), "destroy", gtk_main_quit, NULL);
//   g_signal_connect(G_OBJECT(window), "delete-event", gtk_main_quit, NULL);

//   canvas = gtk_drawing_area_new();
//   gtk_widget_set_size_request(canvas, width, height);
//   status            = g_new(status_t, 1);
//   status->canvas    = canvas;
//   status->magnitude = magnitude;
//   status->up        = true;
//   g_signal_connect(G_OBJECT(canvas), "draw", G_CALLBACK(cb_draw), status);

//   g_timeout_add(50, tick, status);

//   gtk_container_add(GTK_CONTAINER(window), canvas);
//   gtk_widget_show_all(window);

//   gtk_main();

//   // Cleanup here
//   g_free(status);

//   return 0;
// }
