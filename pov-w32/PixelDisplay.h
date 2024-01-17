#pragma once

#include "windows.h"
#include "windowsx.h"

class PixelDisplay
{
public:
  PixelDisplay(HWND hWnd, int height, int width);
  ~PixelDisplay();

  void reset();
  void copyToDC(HDC hdc);

private:
  HWND visibleHwnd;
  HDC memdc;
  int m_width = 16 * 8;
  int m_height = 9 * 8;
};

