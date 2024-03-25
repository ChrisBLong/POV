#include "Face.h"

/*
* Calculates and returns the mean Z coordinate of all the vertices contributing to this face.
*/
float Face::setZMean(const std::vector<Vertex>& vertices)
{
  float sum = 0;

  // Iterate over the vertices by taking the starting vertex of each Edge.
  for (const auto& e : edges) {
    sum += (float)(vertices[e.start].z);
  }

  zMean = sum / edges.size();

  return zMean;
}
