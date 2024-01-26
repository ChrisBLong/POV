#pragma once

#include "windows.h"
#include "windowsx.h"

#include "Cube.h"
#include <string>

class PixelDisplay
{
public:
  PixelDisplay(HWND hWnd, int height, int width);
  ~PixelDisplay();

  void reset();
  void invalidate() { InvalidateRect(visibleHwnd, NULL, false); }
  void copyToDC(HDC hdc, HWND status);
  void toggleEraseBackground() { eraseBackground = !eraseBackground; }

  int getRandom(int max) { return rand() % max; }

  void addOneRandomLine();
  void addRandomLines(int n);

  void startAnimation();
  void stopAnimation();
  void toggleAnimation();
  void nextFrame();

  void enableLine(bool enable) { lineEnabled = enable; }
  void toggleLineEnabled() { lineEnabled = !lineEnabled; }
  void resetLine();
  void moveLine();
  void drawLine();

  void enableCube(bool enable) { cubeEnabled = enable; };
  void toggleCubeEnabled() { cubeEnabled = !cubeEnabled; }
  void resetCube();
  void moveCube();
  void drawCube();
  void translateCube(double _x, double _y, double _z);

  std::wstring getStatusMessage();

private:
  const int timerId = 1234;
  bool timerSet;

  HWND visibleHwnd;
  bool eraseBackground;
  HDC memdc;
  int width;
  int height;

  // For animating the bouncing line.
  bool lineEnabled, lineRunning;
  POINT start, end;
  int vs_x, vs_y, ve_x, ve_y;
 
  // For the spinning cube.
  bool cubeEnabled, cubeRunning;
  Cube cube;
  double dx, dy, dz;
  double rx, ry, rz;

};

