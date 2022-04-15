#include "LayoutReader_GDSIIascii.hpp"

#include <ctime>

LayoutReader_GDSIIascii::LayoutReader_GDSIIascii() : p_activeLibrary(nullptr), p_activeElement(nullptr), p_activeGeometry(nullptr) {
}

bool LayoutReader_GDSIIascii::IsMyFormat(const std::wstring &fName) {
  fileName = fName;

  return false;

  if (fName.substr(fName.find_last_of(L".") + 1) == L"gds")
    return true;
  if (fName.substr(fName.find_last_of(L".") + 1) == L"gdt")
    return true;  
  if (fName.substr(fName.find_last_of(L".") + 1) == L"txt")
    return true;

  return false;
}

bool LayoutReader_GDSIIascii::Read(Layout *layout, LayoutReaderOptions* options) {
  if (!layout)
    return false;
  p_layout = layout;

  file.open(fileName, std::ios::in | std::ios::binary);
  if (!file.is_open())
    return false;


  return false;
}
