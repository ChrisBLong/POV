#include "PixelDisplay.h"
#include <sstream>
#include <bitset>
#include <numbers>

PixelDisplay::PixelDisplay(HWND hWnd, int w, int h, int objFileId) : memdc(hWnd, w, h), textdc(hWnd, w, h), width(w), height(h) {

  visibleHwnd = hWnd;
  pixelRatio = 5;
  eraseBackground = false;
  frameCount = 0;

  // Set up basic random number generation.
  srand((unsigned)memdc.getDC());

  // Create a selection of pens in different colours, one pixel wide.
  penWidth = 0;
  createRainbow(penWidth);
  setCurrentPen(0);

  // Default frame rate.
  frameRateFps = 60;

  // No timer currently running.
  timerSet = false;

  // No bouncing line currently shown.
  lineEnabled = true;
  lineRunning = false;
  start.x = start.y = 0;
  vs_x = vs_y = 0;
  end.x = end.y = 0;
  ve_x = ve_y = 0;

  // No cube currently shown.
  cubeEnabled = true;
  cubeRunning = false;
  resetCube();

  // No text currently shown.
  textEnabled = true;
  textRunning = false;
  textFill = false;
  textPos.x = textPos.y = 0;
  createScaledFont();
  textString = L"Hello";

  // Load the Bad Apple frames.
  //frameSet.loadFrames(L"E:\\Video Editing\\Bad Apple\\frames\\outline-only\\image%04d.png", 6538);
  //frameSet.loadFrames(L"E:\\Video Editing\\Bad Apple\\frames\\full-shapes\\image%04d.png", 6538);
  //frameSet.reset();

  // Run through all the frames and save the outputs.
  //processAllFrames();

  // Prints the RGB values of the selected colours to the debug console.
  //printBinaryXORresults();

  // Data for the 3D objects is stored in a resource. Access the resource, convert it to a string and
  // create a stringstream from it that can be passed to the object loader function.
  // The source file's extension is .obj3d (instead of the standard .obj for Wavefront files) because
  // Git and/or Visual Studio seem to treat .obj files as special.
  std::string objData;
  HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(objFileId), L"OBJECT_FILE");
  if (hRes) {
    HGLOBAL hMem = LoadResource(NULL, hRes);
    if (hMem) {
      DWORD size = SizeofResource(NULL, hRes);
      char* resText = (char*)LockResource(hMem);
      objData.assign(resText, size);
      FreeResource(hMem);
    }
  }

  // Convert the char string to a wchar_t string.
  std::wstring wideObjData(objData.begin(), objData.end());

  // Get a stream based on the string.
  std::wistringstream objectDataStream(wideObjData);

  // Pass the stream to the loader.
  objects = Object3D::loadObjFile(objectDataStream);

  // Add two more objects defined directly in the Object3D class source.
  objects.push_back(Object3D::builtIn(TETRAHEDRON));
  objects.push_back(Object3D::builtIn(COBRA_MKIII));

  // Start with the cube.
  setCurrentObject(1);

  startAnimation();

}

PixelDisplay::~PixelDisplay() {
  stopAnimation();
  unweaveRainbow();
}

void PixelDisplay::createRainbow(int penWidth)
{
  // Clear any existing pens.
  unweaveRainbow();

  //// Create some pretty pens.
  //for (int i = 0; i < 16; i++) {
  //  auto angle = i * 3.14159 / 9;
  //  auto colour = RGB(sin(angle) * 255, sin(angle + 1) * 255, sin(angle + 2) * 255);
  //  pens.push_back(CreatePen(PS_SOLID, penWidth, colour));
  //  colours.push_back(colour);
  //}

  // Per a comment suggestion, try pure RGB, black and white.
  colours.push_back(RGB(255, 0, 0));
  colours.push_back(RGB(0, 255, 0));
  colours.push_back(RGB(0, 0, 255));
  colours.push_back(RGB(0, 0, 0));
  colours.push_back(RGB(255, 255, 255));

  for (auto c : colours) {
    pens.push_back(CreatePen(PS_SOLID, penWidth, c));
  }

}

void PixelDisplay::unweaveRainbow()
{
  // Delete any existing pens.
  for (auto& pen : pens) { if (pen) DeleteObject(pen); }

  // Clear the vectors (colours don't need to be deleted).
  pens.clear();
  colours.clear();
}

void PixelDisplay::setCurrentPen(int i)
{
  if (i < 0 || i >= (int)pens.size()) return;
  currentPenIndex = i;
  SelectObject(memdc, pens[currentPenIndex]);
}

/*
* Changes the colour of the current pen by deleting the existing one and creating a new one.
* Should probably be using GDI+ at this point but we are where we are.
*/
void PixelDisplay::setCurrentPenColour(COLORREF colour)
{
  colours[currentPenIndex] = colour;
  DeleteObject(pens[currentPenIndex]);
  pens[currentPenIndex] = CreatePen(PS_SOLID, penWidth, colour);
  setCurrentPen(currentPenIndex);
}

HPEN PixelDisplay::nextPen()
{
  currentPenIndex = (currentPenIndex + 1) % pens.size();
  setCurrentPen(currentPenIndex);
  return pens[currentPenIndex];
}

HPEN PixelDisplay::prevPen()
{
  currentPenIndex = currentPenIndex - 1;
  if (currentPenIndex < 0) currentPenIndex = pens.size() - 1;
  setCurrentPen(currentPenIndex);
  return pens[currentPenIndex];
}

/*
* Called from the configuration dialog window procedure to display a sample of the currently-selected
* colour in a small rectangular window.
*/
void PixelDisplay::fillWindowWithCurrentPen(HWND hdlg, HWND swatch)
{
  RECT rect;
  GetClientRect(swatch, &rect);
  HDC hdc = GetDCEx(swatch, NULL, DCX_PARENTCLIP);
  IntersectClipRect(hdc, rect.left, rect.top, rect.right, rect.bottom);
  SelectObject(hdc, pens[currentPenIndex]);
  for (int y = 0; y < rect.bottom; y++) {
    MoveToEx(hdc, 0, y, NULL);
    LineTo(hdc, rect.right, y);
  }
  ReleaseDC(swatch, hdc);
}

void PixelDisplay::setLineWidth(int w)
{
  unweaveRainbow();
  penWidth = w;
  createRainbow(penWidth);
  setCurrentPen(currentPenIndex);
}

void PixelDisplay::reset()
{
  // Reset the pens, in case they've been changed by the sliders.
  unweaveRainbow();
  createRainbow(penWidth);

  BitBlt(memdc, 0, 0, width, height, NULL, 0, 0, BLACKNESS);
  SetROP2(memdc, R2_XORPEN);
  setCurrentPen(currentPenIndex);

  SelectObject(textdc, textFont);
  SetTextAlign(textdc, TA_TOP | TA_CENTER);
  SetBkMode(textdc, TRANSPARENT);
}

void PixelDisplay::resizeOffscreenDCs(int pixelRatio)
{
  // Calculate the new width and height based on the current monitor's size and the supplied pixel ratio.
  // It would perhaps make more sense to base this on the actual size of the client area of the window,
  // I'm not sure why I've done it like this...
  HMONITOR hm = MonitorFromWindow(visibleHwnd, MONITOR_DEFAULTTOPRIMARY);
  MONITORINFO mi;
  mi.cbSize = sizeof(MONITORINFO);
  GetMonitorInfo(hm, &mi);
  width = mi.rcWork.right / pixelRatio;
  height = (mi.rcWork.bottom - 50) / pixelRatio;

  memdc.resetSize(width, height);
  textdc.resetSize(width, height);
  createScaledFont();
  resetLine();
  resetText();
  reset();
  // Move the line back to the top left. Could probably scale its current position to match the new
  // screen resolution but let's not overcomplicate things.
  start.x = start.y = 10;
  end.x = end.y = 20;
}

void PixelDisplay::copyToDC(HDC hdc, HWND status)
{
  RECT rectMain, rectStatus;
  GetClientRect(visibleHwnd, &rectMain);
  GetClientRect(status, &rectStatus);
  StretchBlt(hdc, 0, 0, rectMain.right, rectMain.bottom - rectStatus.bottom, memdc, 0, 0, width, height, SRCCOPY);
  if (eraseBackground) BitBlt(memdc, 0, 0, width, height, NULL, 0, 0, BLACKNESS);
}

/*
* Copies the current contents of the off-screen DC to a PNG Image.
*/
Image* PixelDisplay::imageFromMemDc(Image* templateFrame) {
  Image* frame = templateFrame->Clone();
  Gdiplus::Graphics* frameGraphics = Gdiplus::Graphics::FromImage(frame);
  HDC hdc = frameGraphics->GetHDC();
  StretchBlt(hdc, 0, 0, frame->GetWidth(), frame->GetHeight(), memdc, 0, 0, width, height, SRCCOPY);
  frameGraphics->ReleaseHDC(hdc);
  delete frameGraphics;
  return frame;
}

void PixelDisplay::processAllFrames() {

  //return;

  wchar_t filename[MAX_PATH];
  wchar_t message[256];

  // Get the class identifier for the PNG encoder.
  CLSID pngClsid;
  FrameSet::GetEncoderClsid(L"image/png", &pngClsid);

  frameSet.reset();
  int framesToProcess = frameSet.size() * 2; // Run through all input frames twice to get the nice 'undo' effect.
  int framesProcessed = 0;
  Image* thisFrame;
  while (framesProcessed <= framesToProcess) {

    Graphics g(textdc);
    // nextFrame() automatically wraps back to the beginning when it gets to the end.
    g.DrawImage(frameSet.nextFrame().getImage(), Rect(0, 0, width, height));
    BitBlt(memdc, 0, 0, width, height, textdc, 0, 0, SRCINVERT);
    thisFrame = imageFromMemDc(frameSet.templateFrame());

    wsprintf(filename, L"E:\\Video Editing\\Bad Apple\\outputFrames\\out%06d.png", framesProcessed);
    auto result = thisFrame->Save(filename, &pngClsid);

    delete thisFrame;

    framesProcessed++;

    if (framesProcessed % 128 == 0) {
      wsprintf(message, L"Processed %d frames...\n", framesProcessed);
      OutputDebugString(message);
    }
  }
}

void PixelDisplay::addRandomLines(int n)
{
  while (n--) addOneRandomLine();
  invalidate();
}

void PixelDisplay::startAnimation()
{
  // Returns non-zero on success.
  if (SetTimer(visibleHwnd, timerId, (1000/frameRateFps), NULL)) timerSet = true;
  if (lineEnabled) {
    if (start.x == 0 && start.y == 0) resetLine();
    lineRunning = true;
  }
  if (cubeEnabled) {
    //if (dx == 0 && dy == 0) resetCube();
    cubeRunning = true;
  }
  if (textEnabled) {
    if (textPos.x == 0 && textPos.y == 0) resetText();
    textRunning = true;
  }
}

void PixelDisplay::setFrameRate(int fps)
{
  frameRateFps = fps;
  if (timerSet) {
    stopAnimation();
    startAnimation();
  }
}

/* 
  Displays a pattern of crossed lines to show how the colours interact with each other.
*/
void PixelDisplay::testPattern()
{
  stopAnimation();
  reset();
  int strips = pens.size() + 3;
  int spacingX = width / strips;
  int spacingY = height / strips;

  int left = spacingX;
  int right = spacingX * (strips - 1);
  int top = spacingY;
  int bottom = spacingY * (strips - 1);

  for (size_t i = 0; i < pens.size(); i++) {
    // Draw two lines in each colour.
    setCurrentPen(i);
    // Vertical line.
    MoveToEx(memdc, spacingX * (i + 2), top, NULL);
    LineTo(memdc, spacingX * (i + 2), bottom);
    // Horizontal line.
    MoveToEx(memdc, left, spacingY * (i + 2), NULL);
    LineTo(memdc, right, spacingY * (i + 2));
  }

  invalidate();
}

/*
* This function calculates the XOR of each colour with all other colours and writes all the
* information, tab-delimited, to the console using OutputDebugString. I then copy-pasted the
* information in to Excel for use in the YouTube video.
*/
void PixelDisplay::printBinaryXORresults() {
  int col1index = 0;
  for (const auto& col1 : colours) {
    int col2index = 0;
    for (const auto& col2 : colours) {
      auto result = col1 ^ col2;
      std::wstringstream out;
      out << L"\tR\tG\tB\n";
      out << L"Colour " << col1index << "\t";
      addBinaryColorrefToStream(out, col1);
      out << L"\n";
      out << L"Colour " << col2index << "\t";
      addBinaryColorrefToStream(out, col2);
      out << L"\n";
      out << L"Result\t";
      addBinaryColorrefToStream(out, result);
      out << L"\n\n";
      OutputDebugString(out.str().c_str());
      col2index++;
    }
    col1index++;
  }
}

/*
* Breaks the supplied colour in to red, green and blue channels, then writes them to 
* the supplied stream in both binary and decimal.
*/
void PixelDisplay::addBinaryColorrefToStream(std::wstringstream& out, COLORREF col) {
  int red = GetRValue(col);
  int green = GetGValue(col);
  int blue = GetBValue(col);
  out << std::bitset<8>(red) << L" (" << (unsigned int)red << L")\t";
  out << std::bitset<8>(green) << L" (" << (unsigned int)green << L")\t";
  out << std::bitset<8>(blue) << L" (" << (unsigned int)blue << L")";
}

/*
* Stop the on-screen animation by cancelling the timer, if one is currently set.
* The 'lineRunning' etc flags don't seem necessary any more, I can't remember why
* I added them in the first place.
*/
void PixelDisplay::stopAnimation()
{
  if (timerSet) KillTimer(visibleHwnd, timerId);
  timerSet = false;
  //lineRunning = false;
  //cubeRunning = false;
  //textRunning = false;
}

void PixelDisplay::toggleAnimation()
{
  if (timerSet) {
    stopAnimation();
  } else {
    startAnimation();
  }
}

void PixelDisplay::nextFrame()
{
  if (lineEnabled && lineRunning) {
    moveLine();
    drawLine();
  }
  if (cubeEnabled && cubeRunning) {
    moveCube();
    drawCube();
  }
  if (textEnabled && textRunning) {
    moveText();
    drawText();
  }

  //if (frameCount % 10 == 0) {
//    Graphics g(textdc);
//    g.DrawImage(frameSet.nextFrame().getImage(), Rect(0, 0, width, height));
//    BitBlt(memdc, 0, 0, width, height, textdc, 0, 0, SRCINVERT);
  //}

  frameCount++;
  invalidate();
}

void PixelDisplay::resetLine()
{
  // Starting position and velocities scale with resolution, but must be greater than zero.
  int fraction = width / 100;

  if (fraction == 0) fraction = 1;

  start.x = fraction;
  start.y = 0;
  vs_x = getRandom(fraction) + 1;
  vs_y = getRandom(fraction) + 1;

  end.x = 0;
  end.y = 5;
  ve_x = getRandom(fraction) + 1;
  ve_y = getRandom(fraction) + 1;
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

/*
* Updates the current object being drawn on-screen.
*/
void PixelDisplay::setCurrentObject(int i)
{
  if (i < 0 || i > getObjectCount()) return;
  currentObjectIndex = i;
  currentObject = objects[i];
}

void PixelDisplay::resetCube()
{
  dx = dy = 0;
  dz = height * 2;
  rx = ry = rz = 0;
  rotationSpeed = 1.2;
  integralRotation = false;
}

void PixelDisplay::moveCube()
{
  auto delta = rotationSpeed / frameRateFps;

  if (integralRotation) {
    // Adjust delta so that the rotations will get back to as close to zero as possible after
    // a full rotation, which will result in the nice 'undoing' effect.
    constexpr auto twoPi = 2.0 * std::numbers::pi;
    auto framesPerFullRotation = (int)(twoPi / delta);
    delta = twoPi / framesPerFullRotation;
  }

  rx += delta;
  ry += delta;
  rz += delta;

}

void PixelDisplay::drawCube()
{
  Object3D t(currentObject);
  t.rotate(rx, ry, rz);
  t.translate(dx, dy, dz);
  // Second parameter is the distance from the viewer's eye to the virtual screen used in the projection function.
  // Setting equal to the screen's height seems to give the right 'focal length'.
  t.draw(memdc, height, width, height);
}

void PixelDisplay::translateCube(double _x, double _y, double _z)
{
  dx += _x;
  dy += _y;
  dz += _z;
}

void PixelDisplay::resetText()
{
  // Starting position and velocities scale with resolution, but must be greater than zero.
  int fraction = width / 100;

  if (fraction == 0) fraction = 1;

  textPos.x = fraction;
  textPos.y = fraction;

  textVelocity.x = getRandom(fraction) + 1;
  textVelocity.y = getRandom(fraction) + 1;
}

void PixelDisplay::moveText()
{
  // Update the position of the text based on its speed.
  textPos.x += textVelocity.x;
  textPos.y += textVelocity.y;

  // Bounce the text when it reaches the edge of the window.
  if (textPos.x < 0) {
    textPos.x = 0;
    textVelocity.x = -textVelocity.x;
  }
  if (textPos.x > width) {
    textPos.x = width;
    textVelocity.x = -textVelocity.x;
  }
  if (textPos.y < 0) {
    textPos.y = 0;
    textVelocity.y = -textVelocity.y;
  }
  if (textPos.y > height) {
    textPos.y = height;
    textVelocity.y = -textVelocity.y;
  }
}

HFONT PixelDisplay::createScaledFont()
{
  int fontSizePixels = height / 6;
  textFont = CreateFont(-fontSizePixels, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"Arial Black");
  HFONT oldFont = (HFONT) SelectObject(textdc, textFont);
  DeleteObject(oldFont);
  return textFont;
}

void PixelDisplay::drawText()
{
  BitBlt(textdc, 0, 0, width, height, NULL, 0, 0, BLACKNESS);
  auto line = textString.c_str();
  SelectObject(textdc, pens[currentPenIndex]);
  SetDCBrushColor(textdc, colours[currentPenIndex]);
  BeginPath(textdc);
  TextOut(textdc, textPos.x, textPos.y, line, wcslen(line));
  EndPath(textdc);
  if (textFill) {
    StrokeAndFillPath(textdc);
  }
  else {
    StrokePath(textdc);
  }
  BitBlt(memdc, 0, 0, width, height, textdc, 0, 0, SRCINVERT);  
}

std::wstring PixelDisplay::getStatusMessage()
{
  std::wstringstream message;
  message << L"Animation: ";
  message << (timerSet ? L"Yes" : L"No");
  message << L", Background erase?: ";
  message << (eraseBackground ? L"Yes" : L"No");
  message << L", Line enabled: ";
  message << (lineEnabled ? L"Yes" : L"No");
  message << L", Cube enabled: ";
  message << (cubeEnabled ? L"Yes" : L"No");
  message << L", Text enabled: ";
  message << (textEnabled ? L"Yes" : L"No");
  return message.str();
}


void PixelDisplay::addOneRandomLine() {
  // Draw a line from somewhere on the left of the screen to somewhere on the right,
  // not entirely randomly.
  int margin = width / 5;
  MoveToEx(memdc, getRandom(margin), getRandom(height), NULL);
  LineTo(memdc, width - getRandom(margin), getRandom(height));
}
