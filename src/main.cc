#include "compiler.h"

#include <iostream>
#include <cstring>

bool is_help(char* arg) {
	return std::strcmp(arg, "-h") == 0 || std::strcmp(arg, "--help") == 0;
}

int main(int argc, char** argv) {
	if (argc != 2 || is_help(argv[1])) {
		std::cerr << "Usage:" << std::endl;
		std::cerr << argv[0] << " input_file" << std::endl;
		return 1;
	}
  String fname;
  fname.data = argv[1];
  fname.count = strlen(argv[1]);
	Compiler c(fname);
	return c.compile();
}
