#include "Frame.h"

Frame::Frame(std::wstring _f) : filename(_f)
{
  //  Loads a PNG (or JPG or BMP) image from disk.
  image = new Image(filename.c_str());
}
