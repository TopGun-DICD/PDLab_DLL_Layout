/*
 * LayoutReader_GDSIIbin.hpp
 *
 * Layout formats implementation:
 * GDSII Binary - Dmitry A. Bulakh
 */
#pragma once

#include "Layout.hpp"
#include "LayoutReader.hpp"
#include "GDSIITypes.hpp"

class LayoutReader_GDSIIbin : public LayoutReader {
  Library      *p_activeLibrary;
  Element      *p_activeElement;
  Geometry     *p_activeGeometry;
public:
  LayoutReader_GDSIIbin();
public:
  bool IsMyFormat(const std::wstring &fName) final;
  bool Read(Layout *layout, LayoutReaderOptions *options) final;
private:
  void ReadSection_HEADER(GDSIIRecord &_GDSIIRecord);
  void ReadSection_BEGINLIBRARY(GDSIIRecord &_GDSIIRecord);
  void ReadSection_LIBNAME(GDSIIRecord &_GDSIIRecord);
  void ReadSection_UNITS(GDSIIRecord &_GDSIIRecord);
  void ReadSection_ENDLIBRARY(GDSIIRecord &_GDSIIRecord);
  void ReadSection_BEGINSTRUCTURE(GDSIIRecord &_GDSIIRecord);
  void ReadSection_STRNAME(GDSIIRecord &_GDSIIRecord);
  void ReadSection_ENDSTRUCTURE(GDSIIRecord &_GDSIIRecord);
  void ReadSection_BOUNDARY(GDSIIRecord &_GDSIIRecord);
  void ReadSection_PATH(GDSIIRecord &_GDSIIRecord);
  void ReadSection_SREF(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_AREF(GDSIIRecord &_GDSIIRecord);
  void ReadSection_TEXT(GDSIIRecord &_GDSIIRecord);
  void ReadSection_LAYER(GDSIIRecord &_GDSIIRecord);
  void ReadSection_DATATYPE(GDSIIRecord &_GDSIIRecord);
  void ReadSection_WIDTH(GDSIIRecord &_GDSIIRecord);
  void ReadSection_XY(GDSIIRecord &_GDSIIRecord);
  void ReadSection_ENDELEMENT(GDSIIRecord &_GDSIIRecord);
  void ReadSection_SNAME(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_COLROW(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_TEXTNODE(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_NODE(GDSIIRecord &_GDSIIRecord);
  void ReadSection_TEXTTYPE(GDSIIRecord &_GDSIIRecord);
  void ReadSection_PRESENTATION(GDSIIRecord &_GDSIIRecord);
  // UNUSED
  void ReadSection_STRING(GDSIIRecord &_GDSIIRecord);
  void ReadSection_STRANS(GDSIIRecord &_GDSIIRecord);
  void ReadSection_MAG(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_ANGLE(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_LINKTYPE(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_LINKKEYS(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_REFLIBS(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_FONTS(GDSIIRecord &_GDSIIRecord);
  void ReadSection_PATHTYPE(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_GENERATIONS(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_ATTRTABLE(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_STYPTABLE(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_STRTYPE(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_ELFLAGS(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_ELKEY(GDSIIRecord &_GDSIIRecord);
  // UNUSED
  // UNUSED
  //void ReadSection_NODETYPE(GDSIIRecord &_GDSIIRecord);
  void ReadSection_PROPATTR(GDSIIRecord &_GDSIIRecord);
  void ReadSection_PROPVALUE(GDSIIRecord &_GDSIIRecord);
  void ReadSection_BOX(GDSIIRecord &_GDSIIRecord);
  void ReadSection_BOXTYPE(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_PLEX(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_BGNEXTN(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_ENDTEXTN(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_TAPENUM(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_TAPECODE(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_STRCLASS(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_RESERVED(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_FORMAT(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_MASK(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_ENDMASK(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_LIBDIRSIZE(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_SRFNAME(GDSIIRecord &_GDSIIRecord);
  //void ReadSection_LIBSECUR(GDSIIRecord &_GDSIIRecord);

  void ApplyLayoutReaderOptionsToActiveElement();
  bool ResolveReferences();
};