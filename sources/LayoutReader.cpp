#include "LayoutReader.hpp"
#include "LayoutReader_CIF.hpp"
#include "LayoutReader_DXF.hpp"
#include "LayoutReader_GDSIIbin.hpp"
#include "LayoutReader_GDSIIascii.hpp"
#include "LayoutReader_MAG.hpp"
#include "LayoutReader_MSK.hpp"
#include "LayoutReader_OASIS.hpp"

LayoutReader::LayoutReader() : p_layout(nullptr) {}

LayoutReader *GetReader(const std::wstring &fName) {
  LayoutReader *p_reader = nullptr;

  // Check if file is in CIF format
  p_reader = new LayoutReader_CIF;
  if (p_reader->IsMyFormat(fName))
    return p_reader;
  delete p_reader;
  p_reader = nullptr;

  // Check if file is in DXF format
  p_reader = new LayoutReader_DXF;
  if (p_reader->IsMyFormat(fName))
    return p_reader;
  delete p_reader;
  p_reader = nullptr;

  // Check if file is in GDSII binary format
  p_reader = new LayoutReader_GDSIIbin;
  if (p_reader->IsMyFormat(fName))
    return p_reader;
  delete p_reader;
  p_reader = nullptr;

  // Check if file is in GDSII ASCII format
  p_reader = new LayoutReader_GDSIIascii;
  if (p_reader->IsMyFormat(fName))
    return p_reader;
  delete p_reader;
  p_reader = nullptr;

  // Check if file is in MAG format
  p_reader = new LayoutReader_MAG;
  if (p_reader->IsMyFormat(fName))
    return p_reader;
  delete p_reader;
  p_reader = nullptr;

  // Check if file is in MSK format
  p_reader = new LayoutReader_MSK;
  if (p_reader->IsMyFormat(fName))
    return p_reader;
  delete p_reader;
  p_reader = nullptr;

  // Check if file is in OASIS format
  p_reader = new LayoutReader_OASIS;
  if (p_reader->IsMyFormat(fName))
    return p_reader;
  delete p_reader;
  p_reader = nullptr;

  return p_reader;
}

void FreeReader(LayoutReader *reader) {
  if (!reader)
    return;
  delete reader;
  reader = nullptr;
}
