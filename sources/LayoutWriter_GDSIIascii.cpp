/*
 * LayoutWriter_GDSIIBin.cpp
 *
 * Calma binary GDS II file format writer by Dmitry A. Bulakh
 * 20.03.2021
 */

#include "LayoutWriter_GDSIIascii.hpp"
#include "GDSIIHelperFunctions.hpp"

#include <ctime>
#include "GDSIITypes.hpp"

LayoutWriter_GDSIIascii::LayoutWriter_GDSIIascii() {

}

bool LayoutWriter_GDSIIascii::Write(std::wstring fileName, Layout* layout) {
  if (!layout)
    return false;

  return false;
}