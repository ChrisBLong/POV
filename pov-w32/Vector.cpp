#include "Vector.h"

Vector Vector::add(const Vector& other)
{
  return Vector(x + other.x, y + other.y, z + other.z);
}

Vector Vector::subtract(const Vector& other)
{
  return Vector(x - other.x, y - other.y, z - other.z);
}

Vector Vector::cross(const Vector& other)
{
  return Vector(
    (y * other.z) - (z * other.y),
    (z * other.x) - (x * other.z),
    (x * other.y) - (y * other.x)
  );
}
