/*
 * LayoutWriter_OASIS.cpp
 *
 * OASIS file format writer by <author>
 * <date>
 */

#include "LayoutWriter_OASIS.hpp"
#include "GDSIIHelperFunctions.hpp"

#include <ctime>
#include "GDSIITypes.hpp"

LayoutWriter_OASIS::LayoutWriter_OASIS() {

}

bool LayoutWriter_OASIS::Write(std::wstring fileName, Layout *layout) {
  if (!layout)
    return false;
  
  return false;
}


