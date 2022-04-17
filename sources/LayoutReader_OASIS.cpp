#include "LayoutReader_OASIS.hpp"

#include <ctime>

// https://github.com/klayoutmatthias/dump_oas_gds2

LayoutReader_OASIS::LayoutReader_OASIS() : p_activeLibrary(nullptr), p_activeElement(nullptr), p_activeItem(nullptr) {

}

bool LayoutReader_OASIS::IsMyFormat(const std::wstring &fName) {
  fileName = fName;

  if (fName.substr(fName.find_last_of(L".") + 1) == L"oa" || fName.substr(fName.find_last_of(L".") + 1) == L"oas")
    return true;


  return false;
}

bool LayoutReader_OASIS::Read(Layout *layout, LayoutReaderOptions* options) {
  if (!layout)
    return false;
  p_layout = layout;

  return false;

  file.open(fileName, std::ios::in | std::ios::binary);
  if (!file.is_open())
    return false;

}

