#pragma once

#include "Layout.hpp"
#include "LayoutReader.hpp"

class OASISReader : public AbstractLayoutReader {
  Library  *p_activeLibrary;
  Element  *p_activeElement;
  Geometry *p_activeItem;
public:
  OASISReader();
public:
  bool IsMyFormat(const std::wstring &fName) final;
  bool Read(Layout *layout) final;
};
