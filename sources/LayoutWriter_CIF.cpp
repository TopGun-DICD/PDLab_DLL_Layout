/*
 * LayoutWriter_CIF.cpp
 *
 * OASIS file format writer by <author>
 * <date>
 */

#include "LayoutWriter_CIF.hpp"
#include "GDSIIHelperFunctions.hpp"

#include <ctime>
#include "GDSIITypes.hpp"

LayoutWriter_CIF::LayoutWriter_CIF() {

}

bool LayoutWriter_CIF::Write(std::wstring fileName, Layout *layout) {
  if (!layout)
    return false;
  
  return false;
}


