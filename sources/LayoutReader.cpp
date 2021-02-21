#include "LayoutReader.hpp"
#include "LayoutReader_GDSIIbin.hpp"
#include "LayoutReader_GDSIIascii.hpp"
#include "LayoutReader_MSK.hpp"

AbstractLayoutReader::AbstractLayoutReader() : p_data(nullptr) {}

AbstractLayoutReader *GetReader(const std::wstring &fName) {
  AbstractLayoutReader *p_reader = nullptr;

  // Check if file is in GDSII binary format
  p_reader = new GDSIIBinaryReader;
  if (p_reader->IsMyFormat(fName))
    return p_reader;
  delete p_reader;
  p_reader = nullptr;

  // Check if file is in GDSII ASCII format
  p_reader = new GDSIIASCIIReader;
  if (p_reader->IsMyFormat(fName))
    return p_reader;
  delete p_reader;
  p_reader = nullptr;

  // Check if file is in MSK format
  p_reader = new MSKReader;
  if (p_reader->IsMyFormat(fName))
    return p_reader;
  delete p_reader;
  p_reader = nullptr;

  return p_reader;
}

void FreeReader(AbstractLayoutReader *reader) {
  delete reader;
  reader = nullptr;
}
