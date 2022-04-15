#include "LayoutWriter.hpp"

class LayoutWriter_GDSIIbin : public LayoutWriter {
public:
  LayoutWriter_GDSIIbin();
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

  void WriteSection_BOUNDARY(Polygon *polygon);
  void WriteSection_BOX(Rectangle *rectangle);
  void WriteSection_SREF(Reference *reference);
  void WriteSection_PATH(Path *path);
};
