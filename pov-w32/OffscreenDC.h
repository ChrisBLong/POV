#pragma once

#include <Windows.h>

class OffscreenDC
{
public:
  OffscreenDC(HWND _s, int _w, int _h);
  ~OffscreenDC();

  operator HDC() { return hdc; }
  HDC getDC() { return hdc; }

  void createObjects(int _w, int _h);
  void deleteObjects();
  void resetSize(int _w, int _h);

private:
  HWND sourceWindow;
  int width, height;
  HDC hdc;
  HBITMAP hbitmap;

};

