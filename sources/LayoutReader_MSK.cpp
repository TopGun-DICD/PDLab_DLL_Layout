#include "LayoutReader_MSK.hpp"

#include <ctime>

MSKReader::MSKReader() : p_activeLibrary(nullptr), p_activeElement(nullptr), p_activeItem(nullptr) {

}

bool MSKReader::IsMyFormat(const std::wstring &fName) {
  fileName = fName;

  if (fName.substr(fName.find_last_of(L".") + 1) == L"msk")
    return true;


  return false;
}

bool MSKReader::Read(Layout *layout) {
  if (!layout)
    return false;
  p_data = layout;

  return false;

  file.open(fileName, std::ios::in | std::ios::binary);
  if (!file.is_open())
    return false;

}

