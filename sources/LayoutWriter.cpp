#include "LayoutWriter.hpp"
#include "LayoutWriter_CIF.hpp"
#include "LayoutWriter_DXF.hpp"
#include "LayoutWriter_GDSIIbin.hpp"
#include "LayoutWriter_GDSIIascii.hpp"
#include "LayoutWriter_MAG.hpp"
#include "LayoutWriter_MSK.hpp"
#include "LayoutWriter_OASIS.hpp"

LayoutWriter::LayoutWriter() {}

LayoutWriter *GetWriter(FileFormat format) {
  switch (format) {
    case FileFormat::CIF        :  return new LayoutWriter_CIF;
    case FileFormat::DXF        :  return new LayoutWriter_DXF;
    case FileFormat::GDSII_bin  :  return new LayoutWriter_GDSIIbin;
    case FileFormat::GDSII_ascii:  return new LayoutWriter_GDSIIascii;
    case FileFormat::MAG        :  return new LayoutWriter_MAG;
    case FileFormat::MSK        :  return new LayoutWriter_MSK;
    case FileFormat::OASIS      :  return new LayoutWriter_OASIS;
  }

  return nullptr;
}

void FreeWriter(LayoutWriter *writer) {
  if (!writer)
    return;
  delete writer;
  writer = nullptr;
}
