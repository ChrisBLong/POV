#include "OffscreenDC.h"

OffscreenDC::OffscreenDC(HWND _s, int _w, int _h) :
  sourceWindow(_s), width(_w), height(_h)
{
  createObjects(width, height);
}

OffscreenDC::~OffscreenDC() {
  deleteObjects();
}

void OffscreenDC::createObjects(int _w, int _h) {
  width = _w;
  height = _h;
  hdc = CreateCompatibleDC(GetDC(sourceWindow));
  hbitmap = CreateCompatibleBitmap(GetDC(sourceWindow), width, height);
  SelectObject(hdc, hbitmap);
  SelectObject(hdc, GetStockObject(DC_BRUSH));

}

void OffscreenDC::deleteObjects()
{
  DeleteDC(hdc);
  DeleteObject(hbitmap);
}

void OffscreenDC::resetSize(int _w, int _h) {
  deleteObjects();
  createObjects(_w, _h);
}