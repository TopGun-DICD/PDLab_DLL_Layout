#include "LayoutReader_OASIS.hpp"

#include <ctime>

// https://github.com/klayoutmatthias/dump_oas_gds2

OASISReader::OASISReader() : p_activeLibrary(nullptr), p_activeElement(nullptr), p_activeItem(nullptr) {

}

bool OASISReader::IsMyFormat(const std::wstring &fName) {
  fileName = fName;

  if (fName.substr(fName.find_last_of(L".") + 1) == L"oa")
    return true;


  return false;
}

bool OASISReader::Read(Layout *layout, LayoutReaderOptions* options) {
  if (!layout)
    return false;
  p_layout = layout;

  return false;

  file.open(fileName, std::ios::in | std::ios::binary);
  if (!file.is_open())
    return false;

}

