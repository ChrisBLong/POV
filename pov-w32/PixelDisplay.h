#pragma once

#include "windows.h"
#include "windowsx.h"

class PixelDisplay
{
public:
  PixelDisplay(HWND hWnd, int height, int width);
  ~PixelDisplay();

  void reset();
  void invalidate() { InvalidateRect(visibleHwnd, NULL, false); }
  void copyToDC(HDC hdc);

  int getRandom(int max) { return rand() % max; }

  void addOneRandomLine();
  void addRandomLines(int n);

  void startAnimation();
  void stopAnimation();
  void nextFrame();

  void resetLine();
  void moveLine();
  void drawLine();

private:
  const int timerId = 1234;
  bool timerSet;

  HWND visibleHwnd;
  HDC memdc;
  int width;
  int height;

  // For animating the bouncing line.
  POINT start, end;
  int vs_x, vs_y, ve_x, ve_y;
 
};

