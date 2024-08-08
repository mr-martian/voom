#include "compiler.h"

Compiler::Compiler(String start_file) {
  maybe_add_file(start_file);
}

Compiler::~Compiler() {
}

int Compiler::compile() {
  for (auto& cu : compilation_units) {
    std::cout << cu->filename << std::endl;
    cu->tokenize();
    if (cu->errors) {
      std::cout << "HAS ERRORS" << std::endl;
    }
    cu->dumpTokens();
  }
  return 0;
}

void Compiler::maybe_add_file(String filename) {
  std::string s(filename.data, filename.count);
  std::filesystem::path p(s);
  // TODO: search directories
  if (loaded_paths.find(p) != loaded_paths.end()) return;
  loaded_paths.insert(p);
  CompilationUnit* cu = new CompilationUnit(p);
  if (cu->errors) {
    // TODO: report error
  }
  compilation_units.push_back(cu);
}
