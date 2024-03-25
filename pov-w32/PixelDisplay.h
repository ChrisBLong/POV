#pragma once

#include "windows.h"
#include "windowsx.h"

#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include <string>
#include <math.h>

#include "OffscreenDC.h"
#include "Object3D.h"
#include "FrameSet.h"

// For the OKLab color space.
struct csLab { float L; float a; float b; };
struct csRGB { float r; float g; float b; };

class PixelDisplay
{
public:
  PixelDisplay(HWND hWnd, int height, int width, int objFileId);
  ~PixelDisplay();

  HDC getMemDc() { return memdc; }

  void createRainbow(int penWidth);
  void unweaveRainbow();
  void setCurrentPen(int i);
  HPEN getCurrentPen() const { return pens[currentPenIndex]; }
  COLORREF getCurrentPenColour() const { return colours[currentPenIndex]; }
  void setCurrentPenColour(COLORREF colour);
  HPEN nextPen();
  HPEN prevPen();
  void fillWindowWithCurrentPen(HWND hdlg, HWND swatch);
  void setLineWidth(int w);
  int getLineWidth() const { return penWidth; }
  int getTranslateDelta() const { return height / 12; }  // Returns a scaled amount to move the cube on each keypress.

  void reset();
  int getPixelRatio() const { return pixelRatio; }
  void resizeOffscreenDCs(int pixelRatio);
  void invalidate() const { InvalidateRect(visibleHwnd, NULL, false); }
  void copyToDC(HDC hdc, HWND status);
  Image* imageFromMemDc(Image* templateFrame);
  bool getEraseBackground() const { return eraseBackground; }
  void setEraseBackground(bool _b) { eraseBackground = _b; }
  void toggleEraseBackground() { eraseBackground = !eraseBackground; }

  void printBinaryXORresults();
  void addBinaryColorrefToStream(std::wstringstream& out, COLORREF col);
  void testPattern();

  void processAllFrames();

  int getRandom(int max) const { return rand() % max; }

  void addOneRandomLine();
  void addRandomLines(int n);

  void startAnimation();
  int getFrameRate() const { return frameRateFps; }
  void setFrameRate(int fps);
  void stopAnimation();
  void toggleAnimation();
  void nextFrame();

  void enableLine(bool enable) { lineEnabled = enable; }
  bool getLineEnabled() const { return lineEnabled; }
  void toggleLineEnabled() { lineEnabled = !lineEnabled; }
  void resetLine();
  void moveLine();
  void drawLine();

  // This part started out with just a 3D cube but now features a variety of objects. References to 'cube'
  // in the code or comments should now be understood to refer to any of the available 3D objects.
  std::vector<Object3D>& getObjectList() { return objects; }
  int getObjectCount() const { return objects.size(); }
  int getCurrentObjectIndex() const { return currentObjectIndex; }
  void setCurrentObject(int i);
  double getRotationSpeed() const { return rotationSpeed; }
  void setRotationSpeed(double _speed) { rotationSpeed = _speed; }
  bool getIntegralRotation() const { return integralRotation; }
  void setIntegralRotation(bool _ir) { integralRotation = _ir; }
  bool getWireframe() const { return wireframe; }
  void setWireframe(bool _wf) { wireframe = _wf; }
  void enableCube(bool enable) { cubeEnabled = enable; };
  bool getCubeEnabled() const { return cubeEnabled; }
  void toggleCubeEnabled() { cubeEnabled = !cubeEnabled; }
  void resetCube();
  void moveCube();
  void drawCube();
  void translateCube(double _x, double _y, double _z);

  void setTextString(wchar_t* t) { textString.assign(t); }
  const wchar_t* getTextString() { return textString.c_str(); }
  void enableText(bool enable) { textEnabled = enable; };
  HFONT createScaledFont();
  bool getTextEnabled() const { return textEnabled; }
  void toggleTextEnabled() { textEnabled = !textEnabled; }
  bool getTextFill() const { return textFill; }
  void setTextFill(bool f) { textFill = f; }
  void resetText();
  void moveText();
  void drawText();

  std::wstring getStatusMessage();

  // From https://bottosson.github.io/posts/oklab/, type names changed slightly to avoid clashes.
  static csLab linear_srgb_to_oklab(csRGB c);
  static csLab srgb_to_oklab(csRGB c);
  static csRGB oklab_to_linear_srgb(csLab c);
  static csRGB oklab_to_srgb(csLab c);

  COLORREF csRGBtoRGB(csRGB c) { return RGB((char)(255.0 * c.r), (char)(255.0 * c.g), (char)(255.0 * c.b)); }

  static float gammaToLinear(float c);
  static float linearToGamma(float c);

  static float clamp(float in, float min, float max);

  void writeLabValue(csLab lab);
  void writeRgbValue(csRGB rgb);
  //void demoOKLabValues();

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
  std::vector<COLORREF> colours;
  int currentPenIndex;
  int penWidth;

  // For animating the bouncing line.
  bool lineEnabled, lineRunning;
  POINT start, end;
  int vs_x, vs_y, ve_x, ve_y;
 
  // For the spinning cube/object.
  bool cubeEnabled, cubeRunning;
  std::vector<Object3D> objects;
  int currentObjectIndex;
  Object3D currentObject = Object3D::builtIn(CUBE);
  double dx, dy, dz;
  double rx, ry, rz;
  double rotationSpeed; // radians per second
  bool integralRotation;
  bool wireframe;

  // For the moving text.
  bool textEnabled, textRunning, textFill;
  POINT textPos, textVelocity;
  HFONT textFont;
  std::wstring textString;

  // For Bad Apple.
  FrameSet frameSet;

};

