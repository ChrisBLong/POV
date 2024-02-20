#include "FrameSet.h"

FrameSet::FrameSet()
{
  // Load a 1920*1080 template PNG as a starting point for the output frames.
  hdTemplate = Frame(L"E:\\Video Editing\\Bad Apple\\hd-template.png");
}

void FrameSet::loadFrames(std::wstring baseFilename, int numFrames) {

  // Load all the individual frames of the Bad Apple animation.
  wchar_t filename[MAX_PATH];

  for (int f = 0; f <= numFrames; f++) {
    wsprintf(filename, baseFilename.c_str(), f);
    frameList.push_back(Frame(filename));
  }
}

//void FrameSet::saveFrames(std::wstring baseFilename)
//{
//  wchar_t filename[MAX_PATH];
//
//  // Get the class identifier for the PNG encoder.
//  CLSID pngClsid;
//  GetEncoderClsid(L"image/png", &pngClsid);
//
//  int errors = 0;
//  int count = 0;
//
//  for (auto& f : frameList) {
//    wsprintf(filename, baseFilename.c_str(), count);
//    auto result = f.getImage()->Save(filename, &pngClsid);
//    count++;
//    if (result != Ok) errors++;
//  }
//
//  std::wstringstream msg;
//  msg << L"Frames saved: " << count << L", errors: " << errors;
//  OutputDebugString(msg.str().c_str());
//
//}
//
/*
* Copied from MSDN.
*/
int FrameSet::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
  UINT  num = 0;          // number of image encoders
  UINT  size = 0;         // size of the image encoder array in bytes

  ImageCodecInfo* pImageCodecInfo = NULL;

  GetImageEncodersSize(&num, &size);
  if (size == 0)
    return -1;  // Failure

  pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
  if (pImageCodecInfo == NULL)
    return -1;  // Failure

  GetImageEncoders(num, size, pImageCodecInfo);

  for (UINT j = 0; j < num; ++j)
  {
    if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
    {
      *pClsid = pImageCodecInfo[j].Clsid;
      free(pImageCodecInfo);
      return j;  // Success
    }
  }

  free(pImageCodecInfo);
  return -1;  // Failure
}
