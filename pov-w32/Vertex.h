#pragma once

class Vertex
{
public:
  Vertex(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
  Vertex project(double zScreen);

  double x;
  double y;
  double z;
};

