//=============================================================================
// Vector4 class, encapsulating (x,y,z,w) coordinates and supporting functions
// Used for pointers and vectors when they are being multiplied by 4x4 matrices
// Supports float (Vector4), double (Vector4d)
//=============================================================================

#include "Vector4.h"

//================================================================================================
// Template Instatiation
//================================================================================================
// Instantiate this template calss for specific numeric types. Prevents use of other types and
// allows us to put code in this cpp file (normally template code must be all in header file)

template class Vector4T<float>; // Vector4 or Vector4f
template class Vector4T<double>; // Vector4d
// Not supporting int Vector4