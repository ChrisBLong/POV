#pragma once

#include <memory>
#include <string>

#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

class Frame
{
public:
  Frame() : image(nullptr) { };
  Frame(std::wstring _f);
  Frame(Image* i) { image = i; }

  //~Frame() { delete image; }

  Image* getImage() { return image;  }

private:
  std::wstring filename;
  Image* image;
};

