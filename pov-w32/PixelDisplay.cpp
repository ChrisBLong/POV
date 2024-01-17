#include "PixelDisplay.h"

PixelDisplay::PixelDisplay(HWND hWnd, int height, int width) {
  visibleHwnd = hWnd;
  // Create a memory DC that will receive the drawing.
  memdc = CreateCompatibleDC(GetDC(hWnd));
  HBITMAP hbitmap = CreateCompatibleBitmap(GetDC(hWnd), m_width, m_height);
  SelectObject(memdc, hbitmap);
}

PixelDisplay::~PixelDisplay() {
  DeleteObject(memdc);
}

void PixelDisplay::reset()
{
  BitBlt(memdc, 0, 0, m_width, m_height, NULL, 0, 0, BLACKNESS);

  SetROP2(memdc, R2_XORPEN);

  SelectObject(memdc, GetStockObject(WHITE_PEN));

  Rectangle(memdc, 25, 25, m_width - 25, m_height - 25);

  MoveToEx(memdc, 0, 0, NULL);
  LineTo(memdc, 50, 50);
  MoveToEx(memdc, 5, 0, NULL);
  LineTo(memdc, 45, 50);
}

void PixelDisplay::copyToDC(HDC hdc)
{
  RECT rect;
  GetClientRect(visibleHwnd, &rect);
  StretchBlt(hdc, 0, 0, rect.right, rect.bottom, memdc, 0, 0, m_width, m_height, SRCAND);

}
