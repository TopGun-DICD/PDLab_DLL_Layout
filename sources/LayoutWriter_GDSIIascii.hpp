#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "LayoutWriter.hpp"
#include "Layout.hpp"

class LayoutWriter_GDSIIascii : public LayoutWriter {
public:
  LayoutWriter_GDSIIascii();
public:
  bool Write(std::wstring fileName, Layout *layout) final;
private:
};
