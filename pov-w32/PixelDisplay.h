#pragma once

#include "windows.h"
#include "windowsx.h"

#include "Cube.h"

class PixelDisplay
{
public:
  PixelDisplay(HWND hWnd, int height, int width);
  ~PixelDisplay();

  void reset();
  void invalidate() { InvalidateRect(visibleHwnd, NULL, false); }
  void copyToDC(HDC hdc);
  void toggleEraseBackground() { eraseBackground = !eraseBackground; }

  int getRandom(int max) { return rand() % max; }

  void addOneRandomLine();
  void addRandomLines(int n);

  void startAnimation(bool animateLine, bool animateCube);
  void stopAnimation();
  void nextFrame();

  void resetLine();
  void moveLine();
  void drawLine();

  void resetCube();
  void moveCube();
  void drawCube();
  void translateCube(double _x, double _y, double _z);

private:
  const int timerId = 1234;
  bool timerSet;

  HWND visibleHwnd;
  bool eraseBackground;
  HDC memdc;
  int width;
  int height;

  // For animating the bouncing line.
  bool lineRunning;
  POINT start, end;
  int vs_x, vs_y, ve_x, ve_y;
 
  // For the spinning cube.
  bool cubeRunning;
  Cube cube;
  double dx, dy, dz;
  double rx, ry, rz;

};

