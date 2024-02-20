#pragma once
#include <vector>
#include <sstream>

#include "Frame.h"

class FrameSet
{
public:
  FrameSet();

  void loadFrames(std::wstring baseFilename, int numFrames);

  void reset() { iter = frameList.begin(); }
  size_t size() { return frameList.size(); }
  Frame& nextFrame() { if (++iter == frameList.end()) iter = frameList.begin(); return *iter; }
  Frame& prevFrame() { if (--iter == frameList.begin()) iter = frameList.end(); return *iter; }
  Image* templateFrame() { return hdTemplate.getImage(); }

  size_t addFrame(const Frame& f) { frameList.push_back(f); return frameList.size(); }

  //void saveFrames(std::wstring baseFilename);

  static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

private:
  Frame hdTemplate;
  std::vector<Frame> frameList;
  int currentFrameIndex;
  std::vector<Frame>::iterator iter;
};

