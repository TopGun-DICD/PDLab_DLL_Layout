#include "LayoutWriter.hpp"

class GDSIIBinaryWriter : public AbstractLayoutWriter {
public:
  GDSIIBinaryWriter();
public:
  bool Write(std::wstring fileName, Layout *layout) final;
private:
  void WriteSection_HEADER();

  void WriteSection_BEGINLIBRARY();
  void WriteSection_LIBNAME(Library *lib);
  void WriteSection_UNITS(Library *lib);
  void WriteSection_ENDLIBRARY(Library *lib);

  void WriteSection_BEGINSTRUCTURE(Element *element);
  void WriteSection_STRNAME(Element *element);

  void WriteSection_ENDSTRUCTURE(Element *element);

  void WriteSection_BOUNDARY(Geometry_Polygon *polygon);
  void WriteSection_SREF(Geometry_Reference *reference);
  void WriteSection_PATH(Geometry_Path *path);
};
