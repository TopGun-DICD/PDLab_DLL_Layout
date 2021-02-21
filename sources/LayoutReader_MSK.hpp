#pragma once

#include "Layout.hpp"
#include "LayoutReader.hpp"

class MSKReader : public AbstractLayoutReader {
  Library      *p_activeLibrary;
  Element      *p_activeElement;
  GeometryItem *p_activeItem;
public:
  MSKReader();
public:
  bool IsMyFormat(const std::wstring &fName) final;
  bool Read(Layout *layout) final;
};
