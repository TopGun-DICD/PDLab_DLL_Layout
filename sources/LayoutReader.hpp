/*
 * LayoutReader.hpp
 *
 * Layout formats implementation:
 * GDSII Binary - Dmitry A. Bulakh
 *                see LayoutReader_GDSIIBin.hpp
 * MSK          - Mikhail S. Kotlyarov  
 *                see LayoutReader_MSK.hpp
 */
#pragma once

#include <string>
#include <fstream>

#include "Layout.hpp"

class LayoutReader {
protected:
  std::wstring          fileName;
  Layout               *p_layout;
  std::ifstream         file;
  LayoutReaderOptions  *options;
public:
  LayoutReader();
public:
  virtual bool IsMyFormat(const std::wstring &fName) = 0;
  virtual bool Read(Layout *layout, LayoutReaderOptions *options) = 0;
};

LayoutReader * GetReader(const std::wstring &fName);
void FreeReader(LayoutReader *ptr);
