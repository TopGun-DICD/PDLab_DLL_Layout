#include "LayoutWriter.hpp"
#include "LayoutWriter_GDSIIbin.hpp"

AbstractLayoutWriter::AbstractLayoutWriter() {}

AbstractLayoutWriter *GetWriter(FileFormat format) {
  switch(format) {
    case FileFormat::GDSII_bin:
      return  new GDSIIBinaryWriter;
  }
  
  return nullptr;
}

void FreeWriter(AbstractLayoutWriter *writer) {
  if (!writer)
    return;
  delete writer;
  writer = nullptr;
}
