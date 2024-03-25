#pragma once

#include "Vertex.h"

class Vector
{
public:
  Vector() : x(0.0), y(0.0), z(0.0) { }
  Vector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }
  Vector(Vertex v) : x((float)v.x), y((float)v.y), z((float)v.z) { }

  Vector add(const Vector& other);
  Vector subtract(const Vector& other);
  Vector cross(const Vector& other);

  float x, y, z;

};

