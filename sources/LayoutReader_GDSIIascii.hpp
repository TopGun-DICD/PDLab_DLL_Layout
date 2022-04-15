/*
 * LayoutReader.hpp
 *
 * Layout formats implementation:
 * GDSII ASCII  - ???
 */
#pragma once

#include "Layout.hpp"
#include "LayoutReader.hpp"

class LayoutReader_GDSIIascii : public LayoutReader {
  Library      *p_activeLibrary;
  Element      *p_activeElement;
  Geometry     *p_activeGeometry;
public:
  LayoutReader_GDSIIascii();
public:
  bool IsMyFormat(const std::wstring &fName) final;
  bool Read(Layout *layout, LayoutReaderOptions *options) final;
};
