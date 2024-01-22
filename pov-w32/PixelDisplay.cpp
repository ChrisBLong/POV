#include "PixelDisplay.h"

PixelDisplay::PixelDisplay(HWND hWnd, int w, int h) {

  visibleHwnd = hWnd;
  width = w;
  height = h;

  // Create a memory DC that will receive the drawing.
  memdc = CreateCompatibleDC(GetDC(hWnd));
  HBITMAP hbitmap = CreateCompatibleBitmap(GetDC(hWnd), width, height);
  SelectObject(memdc, hbitmap);

  // Set up basic random number generation.
  srand((unsigned)memdc);

  // No timer currently running.
  timerSet = false;

  // No bouncing line currently exists.
  start.x = start.y = 0;
  vs_x = vs_y = 0;
  end.x = end.y = 0;
  ve_x = ve_y = 0;

  // Start the cube on screen.
  cube.translate(0, 0, 1000);
  dx = dy = 0;

}

PixelDisplay::~PixelDisplay() {
  stopAnimation();
  DeleteObject(memdc);
}

void PixelDisplay::reset()
{
  BitBlt(memdc, 0, 0, width, height, NULL, 0, 0, BLACKNESS);
  SetROP2(memdc, R2_NOT);
  SelectObject(memdc, GetStockObject(WHITE_PEN));
}

void PixelDisplay::copyToDC(HDC hdc)
{
  RECT rect;
  GetClientRect(visibleHwnd, &rect);
  StretchBlt(hdc, 0, 0, rect.right, rect.bottom, memdc, 0, 0, width, height, SRCCOPY);

}

void PixelDisplay::addRandomLines(int n)
{
  while (n--) addOneRandomLine();
  invalidate();
}

void PixelDisplay::startAnimation()
{
  // Returns non-zero on success.
  if (SetTimer(visibleHwnd, timerId, 16, NULL)) timerSet = true;
  if (start.x == 0 && start.y == 0) resetLine();
}

void PixelDisplay::stopAnimation()
{
  if (timerSet) KillTimer(visibleHwnd, timerId);
}

void PixelDisplay::nextFrame()
{
  moveLine();
  drawLine();
  dx += 2; dy += 2; dz -= 1;
  Cube t(cube);
  t.translate(dx, dy, dz);
  t.draw(memdc, 100.0, width, height);
  invalidate();
}

void PixelDisplay::resetLine()
{
  start.x = 5;
  start.y = 0;
  vs_x = getRandom(5) + 1;
  vs_y = getRandom(5) + 1;

  end.x = 0;
  end.y = 5;
  ve_x = getRandom(5) + 1;
  ve_y = getRandom(5) + 1;

  dx = dy = 0;
  dz = 0;
}

void PixelDisplay::moveLine()
{
  // Update the position of each endpoint based on their speeds.
  start.x += vs_x;
  start.y += vs_y;
  end.x += ve_x;
  end.y += ve_y;

  // Bounce the endpoints if they reach the edge of the window.
  if (start.x < 0 || start.x > width) vs_x = -vs_x;
  if (start.y < 0 || start.y > height) vs_y = -vs_y;
  if (end.x < 0 || end.x > width) ve_x = -ve_x;
  if (end.y < 0 || end.y > height) ve_y = -ve_y;

}

void PixelDisplay::drawLine()
{
  MoveToEx(memdc, start.x, start.y, NULL);
  LineTo(memdc, end.x, end.y);
}

void PixelDisplay::addOneRandomLine() {
  // Draw a line from somewhere on the left of the screen to somewhere on the right,
  // not entirely randomly.
  int margin = width / 5;
  MoveToEx(memdc, getRandom(margin), getRandom(height), NULL);
  LineTo(memdc, width - getRandom(margin), getRandom(height));
}
