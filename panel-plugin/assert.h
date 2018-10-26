#ifndef XFCE_APPLET_SPEED_ASSERT_H
#define XFCE_APPLET_SPEED_ASSERT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <stdexcept>

#if defined(NDEBUG) || (defined(DEBUG) && DEBUG > 0)

#define assert(expr, msg)                                                      \
  do {                                                                         \
    if(not(expr))                                                              \
      throw std::invalid_argument(msg);                                        \
  } while(0)

#else

#define assert(expr, msg) ((void)0)

#endif // DEBUG
  

#endif // XFCE_APPLET_SPEED_ASSERT_H
