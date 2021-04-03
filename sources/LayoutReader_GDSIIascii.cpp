#include "LayoutReader_GDSIIascii.hpp"

#include <ctime>


// [1] http://boolean.klaasholwerda.nl/interface/bnf/gdsformat.html#recordover
// [2] https://github.com/HomerReid/libGDSII/blob/master/lib/ReadGDSIIFile.cc
// [3] https://github.com/icdop/gds2gdt/blob/main/libsrc/libgds/gdsdata.h
// [4] https://www.artwork.com/gdsii/gdsii/index.htm

union UNION_WORD {
  __int16       value;
  unsigned char byteArray[sizeof(__int16)];
};

union UNION_DWORD {
  __int32       value;
  unsigned char byteArray[sizeof(__int32)];
};

union UNION_DOUBLE {
  double        value;
  unsigned char byteArray[sizeof(double)];
};


GDSIIASCIIReader::GDSIIASCIIReader() : p_activeLibrary(nullptr), p_activeElement(nullptr), p_activeItem(nullptr) {

}

bool GDSIIASCIIReader::IsMyFormat(const std::wstring &fName) {
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

bool GDSIIASCIIReader::Read(Layout *layout) {
  if (!layout)
    return false;
  p_data = layout;

  file.open(fileName, std::ios::in | std::ios::binary);
  if (!file.is_open())
    return false;


  return false;
}
