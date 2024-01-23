#pragma once
#include <Windows.h>
#include <vector>

#include "Vertex.h"
#include "Edge.h"

class Cube
{
public:
  Cube();
  Cube& translate(double dx, double dy, double dz);
  Cube& rotate(double rx, double ry, double rz);
  void draw(HDC memdc, double zScreen, int width, int height);

private:
  std::vector<Vertex> vertices;
  std::vector<Edge> edges;
};

