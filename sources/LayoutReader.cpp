#include "LayoutReader.hpp"

AbstractLayoutReader::AbstractLayoutReader() : p_data(nullptr) {}

AbstractLayoutReader *GetReader(const std::wstring &fName) {
  AbstractLayoutReader *p_reader = nullptr;

  p_reader = new GDSIIBinaryReader;
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
