#ifndef __VOOM_COMPILATION_UNIT_H__
#define __VOOM_COMPILATION_UNIT_H__

#include "string.h"

#include <filesystem>
#include <vector>

enum UnitStatus {
                 UNIT_NULL,
                 UNIT_READ,
                 UNIT_TOKEN,
                 UNIT_PARSE,
                 UNIT_TYPED,
                 UNIT_ERROR,
};

class CompilationUnit {
private:
  char* text = nullptr;
  size_t length = 0;

  enum TokenType {
                  TOKEN_BRACKET,
                  TOKEN_BREAK,
                  TOKEN_CASE,
                  TOKEN_CLASS,
                  TOKEN_COMMA,
                  TOKEN_CONSTANT,
                  TOKEN_CONTINUE,
                  TOKEN_DEFER,
                  TOKEN_DELETE,
                  TOKEN_DO,
                  TOKEN_ELIF,
                  TOKEN_ELSE,
                  TOKEN_ENUM,
                  TOKEN_FOR,
                  TOKEN_IDENT,
                  TOKEN_IMPORT,
                  TOKEN_NUM,
                  TOKEN_OP,
                  TOKEN_RETURN,
                  TOKEN_SEMICOLON,
                  TOKEN_STATEMENT_OP,
                  TOKEN_STR,
                  TOKEN_STRUCT,
                  TOKEN_SWITCH,
                  TOKEN_WHILE,
                  TOKEN_WITH,
                  TOKEN_YIELD,
  };

  enum Operator {
                 // upper 4 bits is precedence, lower is id
                 OP_UNK         = 0x00,
                 OP_ACCESS      = 0x01,
                 OP_COLON       = 0x02,
                 // unary
                 OP_UNARY_PLUS  = 0x10,
                 OP_UNARY_MINUS = 0x11,
                 OP_UNARY_NOT   = 0x12,
                 OP_BIT_NOT     = 0x13,
                 // arithmetic 1
                 OP_MUL         = 0x20,
                 OP_DIV         = 0x21,
                 OP_MOD         = 0x22,
                 // arithmetic 2
                 OP_ADD         = 0x30,
                 OP_SUB         = 0x31,
                 // shifts
                 OP_LSHIFT      = 0x40,
                 OP_RSHIFT      = 0x40,
                 // bitwise
                 OP_BIT_AND     = 0x50,
                 OP_BIT_OR      = 0x51,
                 OP_BIT_XOR     = 0x52,
                 // comparisons
                 OP_LT          = 0x60,
                 OP_LTE         = 0x61,
                 OP_GT          = 0x62,
                 OP_GTE         = 0x63,
                 OP_EQ          = 0x64,
                 OP_NEQ         = 0x65,
                 OP_IN          = 0x66,
                 OP_NOT_IN      = 0x67,
                 // logical
                 OP_NOT         = 0x70,
                 OP_AND         = 0x71,
                 OP_OR          = 0x72,
                 OP_XOR         = 0x73,
                 // process piping
                 OP_PIPE        = 0x80,
                 OP_CHECK_NULL  = 0x81,
                 OP_IF_NULL     = 0x82,
                 // other
                 OP_CAST        = 0x90,
                 // statement types
                 OP_INC         = 0xF0,
                 OP_DEC         = 0xF1,
  };

  struct Token {
    String text;
    size_t line_number = 0;
    size_t byte_number = 0;
    TokenType type;
    Operator op;
  };
  std::vector<Token*> tokens;
  void check_keyword();
  void check_operator();
public:
  std::filesystem::path filename;
  UnitStatus status = UNIT_NULL;
  bool errors = false;
  CompilationUnit(std::filesystem::path filename);
  ~CompilationUnit();
  void tokenize();
  void dumpTokens();
};

#endif
