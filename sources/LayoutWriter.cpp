#include "LayoutWriter.hpp"
#include "LayoutWriter_GDSIIbin.hpp"

LayoutWriter::LayoutWriter() {}

LayoutWriter *GetWriter(FileFormat format) {
  switch (format) {
    case FileFormat::GDSII_bin:  return new LayoutWriter_GDSIIbin;
  }

  return nullptr;
}

void FreeWriter(LayoutWriter *writer) {
  if (!writer)
    return;
  delete writer;
  writer = nullptr;
}
