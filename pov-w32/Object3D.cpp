#include "Object3D.h"
#include <algorithm>

// Create an empty object.
Object3D::Object3D() {

}

/*
* Adjusts the vertices of the object such that its geometric centre is at the origin.
*/
void Object3D::moveToOrigin()
{
  double sumX = 0, sumY = 0, sumZ = 0;
  for (auto& v : vertices) {
    sumX += v.x;
    sumY += v.y;
    sumZ += v.z;
  }
  double centreX = sumX / vertices.size();
  double centreY = sumY / vertices.size();
  double centreZ = sumZ / vertices.size();
  for (auto& v : vertices) {
    v.x -= centreX;
    v.y -= centreY;
    v.z -= centreZ;
  }
}

/*
* Determines the overall size of the object by finding the furthest vertices in each direction,
* then scales the object to make its biggest dimension match the size given. This should only
* be called after moveToOrigin() has already been called for the object, otherwise weird things
* are going to happen.
*/
void Object3D::standardiseScale(double size)
{
  double minX = 1000, minY = 1000, minZ = 1000;
  double maxX = -1000, maxY = -1000, maxZ = -1000;
  for (auto& v : vertices) {
    if (v.x < minX) minX = v.x;
    if (v.y < minY) minY = v.y;
    if (v.z < minZ) minZ = v.z;
    if (v.x > maxX) maxX = v.x;
    if (v.y > maxX) maxY = v.y;
    if (v.z > maxX) maxZ = v.z;
  }
  double sizeX = maxX - minX;
  double sizeY = maxY - minY;
  double sizeZ = maxZ - minZ;
  double currentSize = fmax(fmax(sizeX, sizeY), sizeZ);
  if (currentSize == 0) return;
  double scaleFactor = size / currentSize;
  for (auto& v : vertices) {
    v.x *= scaleFactor;
    v.y *= scaleFactor;
    v.z *= scaleFactor;
  }
}

// Static methods to return certain pre-defined shapes.
Object3D Object3D::builtIn(Object3DShape shape) {

  Object3D obj;

  if (shape == CUBE) {

    obj.name.assign(L"Cube");

    // Create the eight vertices.
    obj.vertices.push_back(Vertex(-100, -100, -100));
    obj.vertices.push_back(Vertex(-100, 100, -100));
    obj.vertices.push_back(Vertex(100, 100, -100));
    obj.vertices.push_back(Vertex(100, -100, -100));
    obj.vertices.push_back(Vertex(-100, -100, 100));
    obj.vertices.push_back(Vertex(-100, 100, 100));
    obj.vertices.push_back(Vertex(100, 100, 100));
    obj.vertices.push_back(Vertex(100, -100, 100));

    // Define the edges to be drawn via the starting and ending vertices.
    obj.edges.push_back(Edge(0, 1));
    obj.edges.push_back(Edge(1, 2));
    obj.edges.push_back(Edge(2, 3));
    obj.edges.push_back(Edge(3, 0));

    obj.edges.push_back(Edge(4, 5));
    obj.edges.push_back(Edge(5, 6));
    obj.edges.push_back(Edge(6, 7));
    obj.edges.push_back(Edge(7, 4));

    obj.edges.push_back(Edge(0, 4));
    obj.edges.push_back(Edge(1, 5));
    obj.edges.push_back(Edge(2, 6));
    obj.edges.push_back(Edge(3, 7));
  }

  if (shape == TETRAHEDRON) {

    obj.name.assign(L"Tetrahedron");

    // Create the four vertices, which are just four corners of a cube.
    obj.vertices.push_back(Vertex(60, 60, 60));
    obj.vertices.push_back(Vertex(-60, 60, -60));
    obj.vertices.push_back(Vertex(60, -60, -60));
    obj.vertices.push_back(Vertex(-60, -60, 60));

    // Define the edges to be drawn via the starting and ending vertices.
    obj.edges.push_back(Edge(0, 1));
    obj.edges.push_back(Edge(0, 2));
    obj.edges.push_back(Edge(0, 3));

    obj.edges.push_back(Edge(1, 2));
    obj.edges.push_back(Edge(2, 3));
    obj.edges.push_back(Edge(3, 1));

  }

  // From the Elite source at https://www.bbcelite.com/master/game_data/variable/ship_cobra_mk_3.html
  if (shape == COBRA_MKIII) {

    obj.name.assign(L"Cobra Mk III");

    obj.vertices.push_back(Vertex(32, 0, 76));
    obj.vertices.push_back(Vertex(-32, 0, 76));
    obj.vertices.push_back(Vertex(0, 26, 24));
    obj.vertices.push_back(Vertex(-120, -3, -8));
    obj.vertices.push_back(Vertex(120, -3, -8));
    obj.vertices.push_back(Vertex(-88, 16, -40));
    obj.vertices.push_back(Vertex(88, 16, -40));
    obj.vertices.push_back(Vertex(128, -8, -40));
    obj.vertices.push_back(Vertex(-128, -8, -40));
    obj.vertices.push_back(Vertex(0, 26, -40));
    obj.vertices.push_back(Vertex(-32, -24, -40));
    obj.vertices.push_back(Vertex(32, -24, -40));
    obj.vertices.push_back(Vertex(-36, 8, -40));
    obj.vertices.push_back(Vertex(-8, 12, -40));
    obj.vertices.push_back(Vertex(8, 12, -40));
    obj.vertices.push_back(Vertex(36, 8, -40));
    obj.vertices.push_back(Vertex(36, -12, -40));
    obj.vertices.push_back(Vertex(8, -16, -40));
    obj.vertices.push_back(Vertex(-8, -16, -40));
    obj.vertices.push_back(Vertex(-36, -12, -40));
    obj.vertices.push_back(Vertex(0, 0, 76));
    obj.vertices.push_back(Vertex(0, 0, 90));
    obj.vertices.push_back(Vertex(-80, -6, -40));
    obj.vertices.push_back(Vertex(-80, 6, -40));
    obj.vertices.push_back(Vertex(-88, 0, -40));
    obj.vertices.push_back(Vertex(80, 6, -40));
    obj.vertices.push_back(Vertex(88, 0, -40));
    obj.vertices.push_back(Vertex(80, -6, -40));

    obj.edges.push_back(Edge(0, 1));
    obj.edges.push_back(Edge(0, 4));
    obj.edges.push_back(Edge(1, 3));
    obj.edges.push_back(Edge(3, 8));
    obj.edges.push_back(Edge(4, 7));
    obj.edges.push_back(Edge(6, 7));
    obj.edges.push_back(Edge(6, 9));
    obj.edges.push_back(Edge(5, 9));
    obj.edges.push_back(Edge(5, 8));
    obj.edges.push_back(Edge(2, 5));
    obj.edges.push_back(Edge(2, 6));
    obj.edges.push_back(Edge(3, 5));
    obj.edges.push_back(Edge(4, 6));
    obj.edges.push_back(Edge(1, 2));
    obj.edges.push_back(Edge(0, 2));
    obj.edges.push_back(Edge(8, 10));
    obj.edges.push_back(Edge(10, 11));
    obj.edges.push_back(Edge(7, 11));
    obj.edges.push_back(Edge(1, 10));
    obj.edges.push_back(Edge(0, 11));
    obj.edges.push_back(Edge(1, 5));
    obj.edges.push_back(Edge(0, 6));
    obj.edges.push_back(Edge(20, 21));
    obj.edges.push_back(Edge(12, 13));
    obj.edges.push_back(Edge(18, 19));
    obj.edges.push_back(Edge(14, 15));
    obj.edges.push_back(Edge(16, 17));
    obj.edges.push_back(Edge(15, 16));
    obj.edges.push_back(Edge(14, 17));
    obj.edges.push_back(Edge(13, 18));
    obj.edges.push_back(Edge(12, 19));
    obj.edges.push_back(Edge(2, 9));
    obj.edges.push_back(Edge(22, 24));
    obj.edges.push_back(Edge(23, 24));
    obj.edges.push_back(Edge(22, 23));
    obj.edges.push_back(Edge(25, 26));
    obj.edges.push_back(Edge(26, 27));
    obj.edges.push_back(Edge(25, 27));
  }

  return obj;
}

/*
  Break a line up on whitespace to a number of separate tokens, returned as a vector.
*/
std::vector<std::wstring> Object3D::tokenise(const std::wstring& str) {

  std::vector<std::wstring> vec;

  auto curChar = str.begin();
  auto end = str.begin();

  while (curChar != str.end()) {

    // Consume whitespace.
    while (curChar != str.end() && std::isspace(*curChar)) curChar++;

    // The next token starts at curChar; now find the last character as well.
    end = curChar;
    while (end != str.end() && !std::isspace(*end)) end++;

    // Substring out the token and add it to the output vector.
    vec.push_back(std::wstring(curChar, end));

    // Start again from the end of the previous token.
    curChar = end;

  }

  return vec;

}

/* Reads a Wavefront-formatted OBJ file and creates one or more Object3D objects corresponding
   to the objects in the file. See https://en.wikipedia.org/wiki/Wavefront_.obj_file
*/
std::vector<Object3D> Object3D::loadObjFile(std::wistream& iStream)
{
  std::vector<Object3D> objList;

  if (!iStream) {
    return objList;
  }

  wchar_t linebuf[MAX_LINE_LENGTH];
  int firstVertexForThisObject = 1;
  Object3D* currObj = nullptr;

  while (iStream.good()) {

    iStream.getline(linebuf, MAX_LINE_LENGTH);

    auto tokens = tokenise(std::wstring(linebuf));
    auto numTokens = tokens.size();

    if (numTokens == 0) continue;

    if (tokens[0] == L"o") {

      // Finalise and store any existing object.
      if (currObj != nullptr) {
        objList.push_back(*currObj);
        // Update the count of vertices processed so far. The 'f' lines defining faces include vertex indices based
        // on counting all the vertices in the whole file, not just the ones for the current object.
        firstVertexForThisObject += currObj->numVertices();
      }

      // Start a new object.
      currObj = new Object3D();
      if (numTokens > 1) currObj->setName(tokens[1].c_str());
    }

    if (currObj && tokens[0] == L"v") {

      // We should have exactly four tokens, for the 'v' and the X, Y and Z coordinates.
      if (numTokens != 4) continue;

      // Add a vertex to the current object.
      currObj->vertices.push_back(Vertex(std::stod(tokens[1]), std::stod(tokens[2]), std::stod(tokens[3])));

    }

    if (currObj && tokens[0] == L"f") {

      // Process a face. Faces are defined by three or more vertex indices. Create Edges linking each listed
      // vertex to the next, including an Edge from the last vertex back to the first. Each vertex
      // index might be part of a group of indices separated by slashes but we only care about the first.
      std::vector<int> indices;

      // Build a list of edges to be transferred to the new Face object later.
      std::vector<Edge> edges;

      for (size_t i = 1; i < numTokens; i++) {
        std::wstring thisToken(tokens[i]);
        auto pos = thisToken.find('/');
        if (pos != std::string::npos) thisToken[pos] = '\0';
        indices.push_back(std::stoi(thisToken) - firstVertexForThisObject);
      }

      // Create edges from one vertex to the next, wrapping around at the end.
      for (size_t i = 0; i < indices.size(); i++) {
        auto endIndex = (i == (indices.size() - 1) ? 0 : i + 1);
        edges.push_back(Edge(indices[i], indices[endIndex]));
      }

      // Add the new Face.
      currObj->faces.push_back(Face(edges));

    }

  }

  // Finalise and store any existing object.
  if (currObj != nullptr) {
    objList.push_back(*currObj);
  }

  // Centre and scale all the objects so that they'll fit on the screen nicely.
  for (auto& o : objList) {
    o.moveToOrigin();
    o.standardiseScale(200.0);
  }

  return objList;
}

Object3D& Object3D::translate(double dx, double dy, double dz) {
  for (auto& v : vertices) {
    v.x += dx;
    v.y += dy;
    v.z += dz;
  }
  return *this;
}

Object3D& Object3D::rotate(double rx, double ry, double rz) {

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

void Object3D::draw(HDC dc, double zScreen, int width, int height, bool wireframe)
{

  // Get a copy of the faces vector.
  std::vector<Face> tFaces(faces);

  // Set the mean Z coordinate for each face.
  for (auto& f : tFaces) f.setZMean(vertices);

  // Sort faces so that the furthest faces get drawn first, and faces closer to
  // the camera obscure the further ones.
  std::ranges::sort(tFaces, [](const Face& a, const Face& b)  { return a.zMean > b.zMean; });

  // Each face wll be outlined with the current pen and filled with black.
  SetDCBrushColor(dc, RGB(0, 0, 0));

  // Iterate over the sorted face vector and draw all the faces.
  for (const auto& f : tFaces) {
    BeginPath(dc);
    bool first = true;
    for (const auto& e : f.edges) {
      if (first) {
        auto vs = vertices[e.start].project(zScreen);
        MoveToEx(dc, ((int)vs.x) + (width / 2), ((int)vs.y) + (height / 2), NULL);
        first = false;
      }
      auto ve = vertices[e.end].project(zScreen);
      LineTo(dc, ((int)ve.x) + (width / 2), ((int)ve.y) + (height / 2));
    }
    EndPath(dc);
    if (wireframe) {
      StrokePath(dc);
    }
    else {
      StrokeAndFillPath(dc);
    }
  }
}

