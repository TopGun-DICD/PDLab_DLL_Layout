#pragma once

#include "Layout.hpp"
#include "LayoutReader.hpp"

struct Record;
class GDSIIASCIIReader : public AbstractLayoutReader {
  Library  *p_activeLibrary;
  Element  *p_activeElement;
  Geometry *p_activeItem;
public:
  GDSIIASCIIReader();
public:
  bool IsMyFormat(const std::wstring &fName) final;
  bool Read(Layout *layout) final;
};
