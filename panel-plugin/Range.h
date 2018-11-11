#ifndef XFCE_APPLET_SPEED_RANGE_H
#define XFCE_APPLET_SPEED_RANGE_H

#include "Debug.h"

template <typename T> class Range {
public:
  class Iterator {
  private:
    T curr;
    T incr;

  public:
    Iterator(T start, T step) : curr(start), incr(step) {
      ;
    }

    bool operator!=(const Iterator& other) const {
      return curr != other.curr;
    }

    const T& operator*() const {
      return curr;
    }

    Iterator& operator++() {
      curr += incr;
      return *this;
    }
  };

private:
  T first;
  T last;
  T step;
  T defalt;

public:
  constexpr Range() : Range(0, 0, 0, 0, true) {
    ;
  }

  constexpr Range(
      T initFirst, T initLast, T initStep, T initDefault, bool inclusive = true)
      : first(initFirst), last(initLast), step(initStep), defalt(initDefault) {
    ASSERT(step != 0, "Non-zero step required for range");

    if(not inclusive) {
      if(step > 0)
        last -= step;
      else if(step < 0)
        last += step;
    }

    if(step > 0)
      ASSERT((first <= last) and (defalt >= first) and (defalt <= last),
             "For step > 0, condition 'first <= default <= last' is violated");
    else
      ASSERT((first >= last) and (defalt <= first) and (defalt >= last),
             "For step < 0, condition 'first >= default >= last' is violated");
  }

  T getFirst() const {
    return first;
  }

  T getLast() const {
    return last;
  }

  T getStep() const {
    return step;
  }

  T getDefault() const {
    return defalt;
  }

  Iterator begin() const {
    return Iterator(first, step);
  }

  Iterator end() const {
    return Iterator(last + step, step);
  }
};

#endif // XFCE_APPLET_SPEED_RANGE_H
