#ifndef XFCE_APPLET_SPEED_DEBUG_H
#define XFCE_APPLET_SPEED_DEBUG_H

#include <gtk/gtk.h>

#include <cstdio>

#if defined(__clang__) || defined(__GNUC__)
#define DEBUG_FUNCNAME __PRETTY_FUNCTION__
#else
#define DEBUG_FUNCNAME __func__
#endif // defined(__GNUC__)

#define ERROR(args...)                                                         \
  do {                                                                         \
    fprintf(stderr, "[%s:%d]: ", __FILE__, __LINE__);                          \
    fprintf(stderr, args);                                                     \
    fprintf(stderr, "\n");                                                     \
    g_error("FATAL ERROR");                                                    \
  } while(0)

#define WARNING(args...)                                                       \
  do {                                                                         \
    g_warning(args);                                                           \
  } while(0)

#if defined(ENABLE_DEBUG)

#define MESSAGE(args...)                                                       \
  do {                                                                         \
    fprintf(stderr, "MESSAGE [%s:%d]: ", __FILE__, __LINE__);                  \
    fprintf(stderr, args);                                                     \
    fprintf(stderr, "\n");                                                     \
  } while(0)

#define ASSERT(expr, msg)                                                      \
  do {                                                                         \
    if(not(expr))                                                              \
      throw std::invalid_argument(msg);                                        \
  } while(0)

#define UNIMPLEMENTED                                                          \
  do {                                                                         \
    g_warning("UNIMPLEMENTED: %s()", DEBUG_FUNCNAME);                          \
  } while(0)

#else // !defined(ENABLE_DEBUG)

#define MESSAGE(...)                                                           \
  do {                                                                         \
    ;                                                                          \
  } while(0)

#define ASSERT(expr, msg)                                                      \
  do {                                                                         \
    ;                                                                          \
  } while(0)

#define UNIMPLEMENTED                                                          \
  do {                                                                         \
    ;                                                                          \
  } while(0)

#endif // defined(ENABLE_DEBUG)

#if defined(ENABLE_TRACE)

#define TRACE_FUNC_ENTER                                                       \
  do {                                                                         \
    fprintf(stderr, "TRACE [%s:%d]: %s (enter)\n", __FILE__, __LINE__,         \
            DEBUG_FUNCNAME);                                                   \
  } while(0)

#define TRACE_FUNC_EXIT                                                        \
  do {                                                                         \
    fprintf(stderr, "TRACE [%s:%d]: %s (exit)\n", __FILE__, __LINE__,          \
            DEBUG_FUNCNAME);                                                   \
  } while(0)

#else

#define TRACE_FUNC_ENTER                                                       \
  do {                                                                         \
    ;                                                                          \
  } while(0)

#define TRACE_FUNC_EXIT                                                        \
  do {                                                                         \
    ;                                                                          \
  } while(0)

#endif // defined(ENABLE_TRACE)

// We keep a sepearate timer trace debug macro to be used in periodically
// executed functions because even while debugging, these can really pollute
// the output, but on occasion, it helps to see what the "tick" functions are
// doing
#if defined(ENABLE_TICK_TRACE)

#define TRACE_TICK_FUNC_ENTER                                                  \
  do {                                                                         \
    fprintf(stderr, "TRACE_TIMER [%s:%d]: %s (enter)\n", __FILE__, __LINE__,   \
            DEBUG_FUNCNAME);                                                   \
  } while(0)

#define TRACE_TICK_FUNC_EXIT                                                   \
  do {                                                                         \
    fprintf(stderr, "TRACE_TIMER [%s:%d]: %s (exit)\n", __FILE__, __LINE__,    \
            DEBUG_FUNCNAME);                                                   \
  } while(0)

#else

#define TRACE_TICK_FUNC_ENTER                                                  \
  do {                                                                         \
    ;                                                                          \
  } while(0)

#define TRACE_TICK_FUNC_EXIT                                                   \
  do {                                                                         \
    ;                                                                          \
  } while(0)

#endif // defined(ENABLE_TICK_TRACE)

#endif // XFCE_APPLET_SPEED_DEBUG_H
