#pragma once

#include <string>
#include <fstream>

#include "Layout.hpp"

class AbstractLayoutWriter {
protected:
  Layout       *p_data;
  std::ofstream file;
public:
  AbstractLayoutWriter();
public:
  virtual bool Write(std::wstring fileName, Layout *layout) = 0;
};

AbstractLayoutWriter *GetWriter(FileFormat format);
void FreeWriter(AbstractLayoutWriter *ptr);