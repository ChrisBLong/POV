#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "Vertex.h"
#include "Edge.h"

#define MAX_LINE_LENGTH 200

enum Object3DShape {
  CUBE, TETRAHEDRON, COBRA_MKIII
};

class Object3D
{
public:
  Object3D();
  void setName(const wchar_t* _name) { name.assign(_name); }
  std::wstring objectName() const { return name; }
  size_t numVertices() const { return vertices.size(); }
  size_t numEdges() const { return edges.size(); }

  void addEdgeWithoutDuplicates(int start, int end);

  Object3D& translate(double dx, double dy, double dz);
  Object3D& rotate(double rx, double ry, double rz);
  void draw(HDC memdc, double zScreen, int width, int height);

  void moveToOrigin();
  void standardiseScale(double size);

  static Object3D builtIn(Object3DShape shape);
  static std::vector<std::wstring> tokenise(const std::wstring& str);
  static std::vector<Object3D> loadObjFile(std::wistream& iStream);

private:
  std::wstring name;
  std::vector<Vertex> vertices;
  std::vector<Edge> edges;
};

