#ifndef XFCE_APPLET_SPEED_RANGE_H
#define XFCE_APPLET_SPEED_RANGE_H

#include "assert.h"

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
  T def;

public:
  constexpr Range() : Range(0, 0, 0, 0, true) {
    ;
  }

  constexpr Range(T pFirst, T pLast, T pStep, T pDef, bool inclusive = true)
      : first(pFirst), last(pLast), step(pStep), def(pDef) {
    assert(step != 0, "Non-zero step required for range");

    if(not inclusive) {
      if(step > 0)
        last -= step;
      else if(step < 0)
        last += step;
    }

    if(step > 0)
      assert((first <= last) and (def >= first) and (def <= last),
             "For step > 0, condition 'first <= def <= last' is violated");
    else 
      assert((first >= last) and (def <= first) and (def >= last),
             "For step < 0, condition 'first >= def >= last' is violated");
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
    return def;
  }

  Iterator begin() const {
    return Iterator(first, step);
  }

  Iterator end() const {
    return Iterator(last + step, step);
  }
};

#endif // XFCE_APPLET_SPEED_RANGE_H
