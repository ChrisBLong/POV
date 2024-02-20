#pragma once

#include "windows.h"
#include "windowsx.h"

#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include <string>
#include <math.h>

#include "OffscreenDC.h"
#include "Cube.h"
#include "FrameSet.h"

class PixelDisplay
{
public:
  PixelDisplay(HWND hWnd, int height, int width);
  ~PixelDisplay();

  void createRainbow(int penWidth);
  void unweaveRainbow();
  void setCurrentPen(int i);
  HPEN nextPen();
  void setLineWidth(int w);
  int getLineWidth() { return penWidth; }
  int getTranslateDelta() { return height / 12; }  // Returns a scaled amount to move the cube on each keypress.

  void reset();
  int getPixelRatio() { return pixelRatio; }
  void resizeOffscreenDCs(int pixelRatio);
  void invalidate() { InvalidateRect(visibleHwnd, NULL, false); }
  void copyToDC(HDC hdc, HWND status);
  Image* imageFromMemDc(Image* templateFrame);
  bool getEraseBackground() { return eraseBackground; }
  void setEraseBackground(bool _b) { eraseBackground = _b; }
  void toggleEraseBackground() { eraseBackground = !eraseBackground; }

  void processAllFrames();

  int getRandom(int max) { return rand() % max; }

  void addOneRandomLine();
  void addRandomLines(int n);

  void startAnimation();
  int getFrameRate() { return frameRateFps; }
  void setFrameRate(int fps);
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

  size_t addLine(std::wstring line);
  void enableText(bool enable) { textEnabled = enable; };
  void toggleTextEnabled() { textEnabled = !textEnabled; }
  void resetText();
  void moveText();
  void nextLine();
  void drawText();

  std::wstring getStatusMessage();

private:
  const int timerId = 1234;
  bool timerSet;
  int frameRateFps;

  HWND visibleHwnd;
  bool eraseBackground;
  OffscreenDC memdc, textdc;
  int pixelRatio;
  int width;
  int height;
  int frameCount;

  // Pens.
  std::vector<HPEN> pens;
  int currentPenIndex;
  int penWidth;

  // For animating the bouncing line.
  bool lineEnabled, lineRunning;
  POINT start, end;
  int vs_x, vs_y, ve_x, ve_y;
 
  // For the spinning cube.
  bool cubeEnabled, cubeRunning;
  Cube cube;
  double dx, dy, dz;
  double rx, ry, rz;

  // For the moving text.
  bool textEnabled, textRunning;
  POINT textPos, textVelocity;
  HFONT textFont;
  std::vector<std::wstring> lines;
  int textLineIndex;

  // For Bad Apple.
  FrameSet frameSet;

};

