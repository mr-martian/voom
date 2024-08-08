#ifndef __VOOM_STRING_H__
#define __VOOM_STRING_H__

#include <cstring>
#include <iostream>

struct String {
  int count = 0;
  char* data = nullptr;
};

inline std::ostream& operator<<(std::ostream& ostr, String s) {
  for (int i = 0; i < s.count; i++) {
    ostr.put(s.data[i]);
  }
  return ostr;
}

inline bool operator==(const String s, const char* c) {
  return std::strncmp(s.data, c, s.count) == 0;
}

#endif
