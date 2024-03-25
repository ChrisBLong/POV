#include "Vertex.h"

Vertex Vertex::project(double zScreen)
{
  Vertex t(*this);

  // Calculate the adjustment factor based on the z distance from the
  // screen, assuming the camera is at z=0.
  double zFactor = zScreen / t.z;

  // Adjust the x and y values by the calculated factor.
  t.x *= zFactor;
  t.y *= zFactor;

  // The z value is irrelevant for the projected vertex.
  t.z = 0;

  return t;
}
