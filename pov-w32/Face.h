#pragma once
#include <vector>

#include "Vector.h"
#include "Edge.h"
#include "Vertex.h"

class Face
{
public:
  Face(std::vector<Edge> _edges) : edges(_edges), zMean(0.0) { }
  float setZMean(const std::vector<Vertex>& vertices);

  std::vector<Edge> edges;
  float zMean;
};

