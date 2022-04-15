#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "LayoutWriter.hpp"
#include "Layout.hpp"

class LayoutWriter_MSK : public LayoutWriter {
public:
  LayoutWriter_MSK();
public:
  bool Write(std::wstring fileName, Layout *layout) final;
private:
};
