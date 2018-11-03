#ifndef XFCE_APPLET_SPEED_TYPES_H
#define XFCE_APPLET_SPEED_TYPES_H

#include "Array.h"

// Useful typedefs are collected here

template <typename Type, typename Dim1> using Array1 = Array<Type, Dim1>;

template <typename Type, typename Dim1, typename Dim2>
using Array2 = Array<Array<Type, Dim2>, Dim1>;

template <typename Type, typename Dim1, typename Dim2, typename Dim3>
using Array3 = Array<Array<Array<Type, Dim3>, Dim2>, Dim1>;

#endif // XFCE_APPLET_SPEED_TYPES_H
