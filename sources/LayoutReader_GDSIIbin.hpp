#pragma once

#include "Layout.hpp"
#include "LayoutReader.hpp"

struct Record;
class GDSIIBinaryReader : public AbstractLayoutReader {
  Library      *p_activeLibrary;
  Element      *p_activeElement;
  GeometryItem *p_activeItem;
public:
  GDSIIBinaryReader();
public:
  bool IsMyFormat(const std::wstring &fName) final;
  bool Read(Layout *layout) final;
private:
  void ReadSection_HEADER(Record &_record);
  void ReadSection_BEGINLIBRARY(Record &_record);
  void ReadSection_LIBNAME(Record &_record);
  void ReadSection_UNITS(Record &_record);
  void ReadSection_ENDLIBRARY(Record &_record);
  void ReadSection_BEGINSTRUCTURE(Record &_record);
  void ReadSection_STRNAME(Record &_record);
  void ReadSection_ENDSTRUCTURE(Record &_record);
  void ReadSection_BOUNDARY(Record &_record);
  void ReadSection_PATH(Record &_record);
  void ReadSection_SREF(Record &_record);
  //void ReadSection_AREF(Record &_record);
  void ReadSection_TEXT(Record &_record);
  void ReadSection_LAYER(Record &_record);
  void ReadSection_DATATYPE(Record &_record);
  void ReadSection_WIDTH(Record &_record);
  void ReadSection_XY(Record &_record);
  void ReadSection_ENDELEMENT(Record &_record);
  void ReadSection_SNAME(Record &_record);
  //void ReadSection_COLROW(Record &_record);
  //void ReadSection_TEXTNODE(Record &_record);
  //void ReadSection_NODE(Record &_record);
  void ReadSection_TEXTTYPE(Record &_record);
  void ReadSection_PRESENTATION(Record &_record);
  // UNUSED
  void ReadSection_STRING(Record &_record);
  void ReadSection_STRANS(Record &_record);
  void ReadSection_MAG(Record &_record);
  //void ReadSection_ANGLE(Record &_record);
  //void ReadSection_LINKTYPE(Record &_record);
  //void ReadSection_LINKKEYS(Record &_record);
  //void ReadSection_REFLIBS(Record &_record);
  //void ReadSection_FONTS(Record &_record);
  void ReadSection_PATHTYPE(Record &_record);
  //void ReadSection_GENERATIONS(Record &_record);
  //void ReadSection_ATTRTABLE(Record &_record);
  //void ReadSection_STYPTABLE(Record &_record);
  //void ReadSection_STRTYPE(Record &_record);
  //void ReadSection_ELFLAGS(Record &_record);
  //void ReadSection_ELKEY(Record &_record);
  // UNUSED
  // UNUSED
  //void ReadSection_NODETYPE(Record &_record);
  void ReadSection_PROPATTR(Record &_record);
  void ReadSection_PROPVALUE(Record &_record);
  void ReadSection_BOX(Record &_record);
  void ReadSection_BOXTYPE(Record &_record);
  //void ReadSection_PLEX(Record &_record);
  //void ReadSection_BGNEXTN(Record &_record);
  //void ReadSection_ENDTEXTN(Record &_record);
  //void ReadSection_TAPENUM(Record &_record);
  //void ReadSection_TAPECODE(Record &_record);
  //void ReadSection_STRCLASS(Record &_record);
  //void ReadSection_RESERVED(Record &_record);
  //void ReadSection_FORMAT(Record &_record);
  //void ReadSection_MASK(Record &_record);
  //void ReadSection_ENDMASK(Record &_record);
  //void ReadSection_LIBDIRSIZE(Record &_record);
  //void ReadSection_SRFNAME(Record &_record);
  //void ReadSection_LIBSECUR(Record &_record);

  bool ResolveReferences();
};
