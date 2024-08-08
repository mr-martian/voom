#ifndef __VOOM_COMPILER_H__
#define __VOOM_COMPILER_H__

#include "compilation_unit.h"

#include <map>
#include <set>
#include <vector>

class Compiler {
private:
  std::vector<CompilationUnit*> compilation_units;
  std::set<std::filesystem::path> loaded_paths;

  void maybe_add_file(String filename);
public:
  Compiler(String start_file);
  ~Compiler();
  int compile();
};

#endif
