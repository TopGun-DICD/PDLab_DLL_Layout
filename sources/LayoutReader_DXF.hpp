#pragma once

#include "Layout.hpp"
#include "LayoutReader.hpp"

class LayoutReader_DXF : public LayoutReader {
  Library  *p_activeLibrary;
  Element  *p_activeElement;
  Geometry *p_activeItem;
public:
  LayoutReader_DXF();
public:
  bool IsMyFormat(const std::wstring &fName) final;
  bool Read(Layout *layout, LayoutReaderOptions* options) final;
};
