#include "PixelDisplay.h"
#include <sstream>

PixelDisplay::PixelDisplay(HWND hWnd, int w, int h) : memdc(hWnd, w, h), textdc(hWnd, w, h), width(w), height(h) {

  visibleHwnd = hWnd;
  pixelRatio = 5;
  eraseBackground = false;
  frameCount = 0;

  // Set up basic random number generation.
  srand((unsigned)memdc.getDC());

  // Create a selection of pens in different colours, one pixel wide.
  penWidth = 0;
  createRainbow(penWidth);

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
  textEnabled = false;
  textRunning = false;
  textPos.x = textPos.y = 0;
  textFont = CreateFont(48, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, L"System");
  addLine(L"My");
  addLine(L"password");
  addLine(L"is");
  addLine(L"-redacted-");
  textLineIndex = 0;

  // Load the Bad Apple frames.
  //frameSet.loadFrames(L"E:\\Video Editing\\Bad Apple\\frames\\outline-only\\image%04d.png", 6538);
  //frameSet.loadFrames(L"E:\\Video Editing\\Bad Apple\\frames\\full-shapes\\image%04d.png", 6538);
  //frameSet.reset();

  // Run through all the frames and save the outputs.
  //processAllFrames();

}

PixelDisplay::~PixelDisplay() {
  stopAnimation();
  unweaveRainbow();
}

void PixelDisplay::createRainbow(int penWidth)
{
  // Clear any existing pens.
  unweaveRainbow();

  // Create some pretty pens.
  for (int i = 0; i < 16; i++) {
    auto angle = i * 3.14159 / 7;
    pens.push_back(CreatePen(PS_SOLID, penWidth, RGB(sin(angle) * 255, sin(angle + 1) * 255, sin(angle + 2) * 255)));
  }
}

void PixelDisplay::unweaveRainbow()
{
  // Delete any existing pens.
  for (auto& pen : pens) { if (pen) DeleteObject(pen); }

  // Clear the vector.
  pens.clear();
}

void PixelDisplay::setCurrentPen(int i)
{
  if (i < 0 || i >= (int)pens.size()) return;
  currentPenIndex = i;
  SelectObject(memdc, pens[currentPenIndex]);
}

HPEN PixelDisplay::nextPen()
{
  currentPenIndex = (currentPenIndex + 1) % pens.size();
  setCurrentPen(currentPenIndex);
  return pens[currentPenIndex];
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
  BitBlt(memdc, 0, 0, width, height, NULL, 0, 0, BLACKNESS);
  SetROP2(memdc, R2_XORPEN);
  setCurrentPen(3);

  SelectObject(textdc, textFont);
  //SelectObject(textdc, GetStockObject(WHITE_PEN));

  SetTextColor(textdc, RGB(255, 255, 255));
  SetTextAlign(textdc, TA_TOP | TA_CENTER);
  SetBkMode(textdc, TRANSPARENT);
}

void PixelDisplay::resizeOffscreenDCs(int pixelRatio)
{
  // Calculate the new width and height based on the desktop size and the supplied pixel ratio.
  width = 1920 / pixelRatio;
  height = 1080 / pixelRatio;
  memdc.resetSize(width, height);
  textdc.resetSize(width, height);
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

void PixelDisplay::stopAnimation()
{
  if (timerSet) KillTimer(visibleHwnd, timerId);
  timerSet = false;
  lineRunning = false;
  cubeRunning = false;
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
  start.x = 5;
  start.y = 0;
  vs_x = getRandom(5) + 1;
  vs_y = getRandom(5) + 1;

  end.x = 0;
  end.y = 5;
  ve_x = getRandom(5) + 1;
  ve_y = getRandom(5) + 1;
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

void PixelDisplay::resetCube()
{
  dx = dy = 0;
  dz = height * 2;
  rx = ry = rz = 0;
}

void PixelDisplay::moveCube()
{
  rx += 0.04;
  ry += 0.03;
  rz += 0.02;
}

void PixelDisplay::drawCube()
{
  Cube t(cube);
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

size_t PixelDisplay::addLine(std::wstring line)
{
  lines.push_back(line);
  return lines.size();
}

void PixelDisplay::resetText()
{
  textPos.x = 20;
  textPos.y = height;
  textVelocity.x = 0;
  textVelocity.y = -2;
}

void PixelDisplay::moveText()
{
  // Temporarily removed this - having the text static seems easier to read.
  //// Update the position of the text based on its speed.
  //textPos.x += textVelocity.x;
  //textPos.y += textVelocity.y;

  //// When the text gets to the top, move it back to the bottom and move on to the next line.
  //if (textPos.y < -20) {
  //  textPos.y = height;
  //  textLineIndex++;
  //  if (textLineIndex == lines.size()) textLineIndex = 0;
  //}
}

void PixelDisplay::nextLine() {
  textLineIndex++;
  if (textLineIndex == lines.size()) textLineIndex = 0;
}

void PixelDisplay::drawText()
{
  // Draw a few frames of text in the middle of the screen every 60 frames (ie every second).
  auto mod = frameCount % 60;
  if (mod > 0 && mod < 5) {
    BitBlt(textdc, 0, 0, width, height, NULL, 0, 0, BLACKNESS);
    auto line = lines[textLineIndex].c_str();
    TextOut(textdc, width / 2, height / 2, line, wcslen(line));
    BitBlt(memdc, 0, 0, width, height, textdc, 0, 0, SRCINVERT);
  } else if (mod == 0) nextLine();
  
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
