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
  T First;
  T Last;
  T Step;
  T Steps;
  T Default;

public:
  constexpr Range() : Range(0, 0, 0, 0, true) {
    ;
  }

  constexpr Range(T first, T last, T step, T def, bool inclusive = true)
      : First(first), Last(last), Step(step), Steps(0), Default(def) {
    ASSERT(Step != 0, "Non-zero step required for range");

    if(not inclusive) {
      if(Step > 0)
        Last -= Step;
      else if(Step < 0)
        Last += Step;
    }

    Steps = (Last - First) / Step;

    if(Step > 0)
      ASSERT((First <= Last) and (Default >= First) and (Default <= Last),
             "For step > 0, condition 'first <= default <= last' is violated");
    else
      ASSERT((First >= Last) and (Default <= First) and (Default >= Last),
             "For step < 0, condition 'first >= default >= last' is violated");
  }

  Iterator begin() const {
    return Iterator(First, Step);
  }

  Iterator end() const {
    return Iterator(Last + Step, Step);
  }
};

#endif // XFCE_APPLET_SPEED_RANGE_H
