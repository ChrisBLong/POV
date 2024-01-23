#include "Cube.h"

// Creates a cube of side 200 pixels centered on the origin.
Cube::Cube() {

  // Create the eight vertices.
  vertices.push_back(Vertex(-100, -100, -100));
  vertices.push_back(Vertex(-100, 100, -100));
  vertices.push_back(Vertex(100, 100, -100));
  vertices.push_back(Vertex(100, -100, -100));
  vertices.push_back(Vertex(-100, -100, 100));
  vertices.push_back(Vertex(-100, 100, 100));
  vertices.push_back(Vertex(100, 100, 100));
  vertices.push_back(Vertex(100, -100, 100));

  // Define the edges to be drawn via the starting and ending vertices.
  edges.push_back(Edge(0, 1));
  edges.push_back(Edge(1, 2));
  edges.push_back(Edge(2, 3));
  edges.push_back(Edge(3, 0));

  edges.push_back(Edge(4, 5));
  edges.push_back(Edge(5, 6));
  edges.push_back(Edge(6, 7));
  edges.push_back(Edge(7, 4));

  edges.push_back(Edge(0, 4));
  edges.push_back(Edge(1, 5));
  edges.push_back(Edge(2, 6));
  edges.push_back(Edge(3, 7));

}

Cube& Cube::translate(double dx, double dy, double dz) {
  for (auto& v : vertices) {
    v.x += dx;
    v.y += dy;
    v.z += dz;
  }
  return *this;
}

Cube& Cube::rotate(double rx, double ry, double rz) {

  auto rot = [](double& a, double& b, double angle) {
    double ta = a * cos(angle) - b * sin(angle);
    b = a * sin(angle) + b * cos(angle);
    a = ta;
    };

  for (auto& v : vertices) {
    rot(v.x, v.y, rz);
    rot(v.x, v.z, ry);
    rot(v.y, v.z, rx);
  }

  return *this;
}

void Cube::draw(HDC memdc, double zScreen, int width, int height)
{
  for (const auto& e : edges) {
    auto vs = vertices[e.start].project(zScreen);
    auto ve = vertices[e.end].project(zScreen);
    MoveToEx(memdc, ((int)vs.x) + (width / 2), ((int)vs.y) + (height / 2), NULL);
    LineTo(memdc, ((int)ve.x) + (width / 2), ((int)ve.y) + (height / 2));
  }
}

