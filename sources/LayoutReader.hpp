#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "Layout.hpp"

class AbstractLayoutReader {
protected:
  std::wstring  fileName;
  Layout       *p_data;
  std::ifstream file;
public:
  AbstractLayoutReader();
public:
  virtual bool IsMyFormat(const std::wstring &fName) = 0;
  virtual bool Read(Layout *layout) = 0;
};

AbstractLayoutReader *GetReader(const std::wstring &fName);
void FreeReader(AbstractLayoutReader *ptr);
