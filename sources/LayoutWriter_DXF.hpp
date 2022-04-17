#pragma once

#include <string>

#include "LayoutWriter.hpp"
#include "Layout.hpp"

class LayoutWriter_DXF : public LayoutWriter {
public:
  LayoutWriter_DXF();
public:
  bool Write(std::wstring fileName, Layout* layout) final;
private:
};

