#include "compilation_unit.h"

CompilationUnit::CompilationUnit(std::filesystem::path filename) {
  this->filename = filename;
  std::error_code ec;
  length = std::filesystem::file_size(filename, ec);
  if (ec.value()) {
    std::cerr << "Unable to stat " << filename << std::endl;
    status = UNIT_ERROR;
    this->errors = true;
  } else {
    text = new char[length];
    FILE* fin = fopen(filename.c_str(), "rb");
    auto read = fread(text, sizeof(char), length, fin);
    fclose(fin);
    if (read != length) {
      std::cerr << "Unable to read all of " << filename << std::endl;
      status = UNIT_ERROR;
      this->errors = true;
    } else {
      status = UNIT_READ;
    }
  }
}

CompilationUnit::~CompilationUnit() {
  delete[] text;
  for (auto& tok : tokens) {
    delete tok;
  }
}

enum TokenizerState {
                     STATE_NULL,
                     STATE_OP,
                     STATE_BRACKET,
                     STATE_IDENT,
                     STATE_NUM,
                     STATE_STR,
                     STATE_STR_ESC,
                     STATE_STR_END,
                     STATE_SEMICOLON,
                     STATE_COMMA,
                     STATE_COMMENT,
};

#define END_TOKEN(typ) {              \
  Token* t = new Token;               \
  t->text.data = start;               \
  t->text.count = (i - start_index);  \
  t->line_number = start_line_number; \
  t->byte_number = start_index;       \
  t->type = (typ);                    \
  tokens.push_back(t);                \
  state = STATE_NULL;                 \
}

#define BEGIN_TOKEN(st) {             \
  start = &text[i];                   \
  start_line_number = line_number;    \
  start_index = i;                    \
  state = (st);                       \
}

enum CharType {
               CHAR_SPACE,
               CHAR_PUNCT,
               CHAR_BRACKET,
               CHAR_IDENT,
               CHAR_NUM,
               CHAR_ESC,
               CHAR_SEMICOLON,
               CHAR_COMMA,
               CHAR_QUOTE,
               CHAR_PERIOD,
               CHAR_COMMENT,
};

CharType get_type(char c) {
  if (c <= ' ' || c == 0x7f) return CHAR_SPACE;
  else if ('0' <= c && c <= '9') return CHAR_NUM;
  else if (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) return CHAR_IDENT;
  else if (c == '_' || c & 0x80) return CHAR_IDENT;
  else if (c == '(' || c == ')') return CHAR_BRACKET;
  else if (c == '[' || c == ']') return CHAR_BRACKET;
  else if (c == '{' || c == '}') return CHAR_BRACKET;
  else if (c == '\\') return CHAR_ESC;
  else if (c == ';') return CHAR_SEMICOLON;
  else if (c == ',') return CHAR_COMMA;
  else if (c == '"') return CHAR_QUOTE;
  else if (c == '.') return CHAR_PERIOD;
  else if (c == '#') return CHAR_COMMENT;
  else return CHAR_PUNCT;
}

void CompilationUnit::check_keyword() {
  Token* tok = tokens.back();
  String ident = tok->text;
  char* buf = ident.data;
  int len = ident.count;
  if (len < 2 || len > 10) return;
  switch (buf[0]) {
  case 'a':
    switch (buf[1]) {
    case 'n':
      if (len == 3 && buf[2] == 'd') { // and
        tok->type = TOKEN_OP;
        tok->op = OP_AND;
      } break;
    case 's':
      if (len == 2) { // as
        tok->type = TOKEN_OP;
        tok->op = OP_CAST;
      } break;
    } break;
  case 'b':
    if (ident == "break") tok->type = TOKEN_BREAK;
    break;
  case 'c':
    switch (buf[1]) {
    case 'a':
      if (ident == "case") tok->type = TOKEN_CASE;
      break;
    case 'l': // TODO: do we need this?
      if (ident == "class") tok->type = TOKEN_CLASS;
      break;
    case 'o':
      if (ident == "continue") tok->type = TOKEN_CONTINUE;
      break;
    } break;
  case 'd': break;
    switch (buf[1]) {
    case 'e':
      if (ident == "defer") tok->type = TOKEN_DEFER;
      else if (ident == "delete") tok->type = TOKEN_DELETE;
      break;
    case 'o':
      if (len == 2) { // do
        tok->type = TOKEN_DO;
      } break;
    } break;
  case 'e': break;
    if (len == 4) {
      switch (buf[1]) {
      case 'l':
        if (ident == "elif") tok->type = TOKEN_ELIF;
        else if (ident == "else") tok->type = TOKEN_ELSE;
        break;
      case 'n':
        if (ident == "enum") tok->type = TOKEN_ENUM;
        break;
      }
    } break;
  case 'f': break;
    switch (buf[1]) {
    case 'a':
      if (ident == "false") tok->type = TOKEN_CONSTANT;
      break;
    case 'o':
      if (ident == "for") tok->type = TOKEN_FOR;
      break;
    } break;
  case 'i':
    switch (buf[1]) {
    case 'm':
      if (ident == "import") tok->type = TOKEN_IMPORT;
      break;
    case 'n':
      if (len == 2) {
        tok->type = TOKEN_OP;
        tok->op = OP_IN;
      } break;
    } break;
  case 'n': break;
    if (ident == "not") {
      tok->type = TOKEN_OP;
      tok->op = OP_NOT;
    } else if (ident == "null") {
      tok->type = TOKEN_CONSTANT;
    } break;
  case 'o':
    if (len == 2 && buf[1] == 'r') { // or
      tok->type = TOKEN_OP;
      tok->op = OP_OR;
    } break;
  case 'r':
    if (ident == "return") tok->type = TOKEN_RETURN;
    break;
  case 's':
    if (ident == "struct") tok->type = TOKEN_STRUCT;
    else if (ident == "switch") tok->type = TOKEN_SWITCH;
    break;
  case 't':
    if (ident == "true") tok->type = TOKEN_CONSTANT;
    break;
  case 'w':
    if (ident == "while") tok->type = TOKEN_WHILE;
    else if (ident == "with") tok->type = TOKEN_WITH;
    break;
  case 'x':
    if (ident == "xor") {
      tok->type = TOKEN_OP;
      tok->op = OP_XOR;
    } break;
  case 'y':
    if (ident == "yield") tok->type = TOKEN_YIELD;
    break;
  }
}

#define SPLIT_TOKEN(idx) {                         \
  if (idx > len) {                                 \
    Token* tok2 = new Token;                       \
    tok2->text.data = &(tok->text.data[(idx)]);    \
    tok2->text.count = (tok->text.count - (idx));  \
    tok2->line_number = tok->line_number;          \
    tok2->byte_number = tok->byte_number + (idx);  \
    tok2->type = TOKEN_OP;                         \
    tokens.push_back(tok2);                        \
    check_operator();                              \
    return;                                        \
  }                                                \
}

#define MAYBE_ASSIGN(idx) {           \
  if (len > (idx)) {                  \
    if (buf[(idx)] == '=') {          \
      tok->type = TOKEN_STATEMENT_OP; \
      SPLIT_TOKEN((idx)+1);           \
    } else SPLIT_TOKEN((idx));        \
  }                                   \
}

void CompilationUnit::check_operator() {
  Token* tok = tokens.back();
  String ident = tok->text;
  char* buf = ident.data;
  int len = ident.count;
  tok->op = OP_UNK;
  switch (buf[0]) {
  case '!':
    if (len > 1 && buf[1] == '=') {
      tok->op = OP_NEQ;
      SPLIT_TOKEN(2);
    } else SPLIT_TOKEN(1);
    break;
  case '%':
    tok->op = OP_MOD;
    MAYBE_ASSIGN(1);
    break;
  case '&':
    tok->op = OP_BIT_AND;
    if (len > 1) {
      if (buf[1] == '&') {
        tok->op = OP_AND;
        MAYBE_ASSIGN(2);
      } else {
        MAYBE_ASSIGN(1);
      }
    } break;
  case '*':
    tok->op = OP_MUL;
    MAYBE_ASSIGN(1);
    break;
  case '+':
    tok->op = OP_ADD;
    if (len > 1 && buf[1] == '+') {
      tok->op = OP_INC;
      tok->type = TOKEN_STATEMENT_OP;
      SPLIT_TOKEN(2);
    } else MAYBE_ASSIGN(1);
    break;
  case '-':
    tok->op = OP_SUB;
    if (len > 1 && buf[1] == '-') {
      tok->op = OP_DEC;
      tok->type = TOKEN_STATEMENT_OP;
      SPLIT_TOKEN(2);
    } else MAYBE_ASSIGN(1);
    break;
  case '.':
    tok->op = OP_ACCESS;
    SPLIT_TOKEN(1);
    break;
  case '/':
    tok->op = OP_DIV;
    MAYBE_ASSIGN(1);
    break;
  case ':':
    tok->op = OP_COLON;
    SPLIT_TOKEN(1);
    break;
  case '<':
    if (len == 1) tok->op = OP_LT;
    else {
      switch (buf[1]) {
      case '<':
        tok->op = OP_LSHIFT;
        MAYBE_ASSIGN(2);
        break;
      case '=':
        tok->op = OP_LTE;
        SPLIT_TOKEN(2);
        break;
      default:
        SPLIT_TOKEN(1);
      }
    } break;
  case '=':
    if (len > 1 && buf[1] == '=') {
      tok->op = OP_EQ;
      SPLIT_TOKEN(2);
    } else {
      tok->type = TOKEN_STATEMENT_OP;
      SPLIT_TOKEN(1);
    }
    break;
  case '>':
    if (len == 1) tok->op = OP_GT;
    else {
      switch (buf[1]) {
      case '>':
        tok->op = OP_RSHIFT;
        MAYBE_ASSIGN(2);
        break;
      case '=':
        tok->op = OP_GTE;
        SPLIT_TOKEN(2);
        break;
      default:
        SPLIT_TOKEN(1);
      }
    } break;
  case '?':
    tok->op = OP_CHECK_NULL;
    if (len > 1 && buf[1] == '?') {
      tok->op = OP_IF_NULL;
      SPLIT_TOKEN(2);
    } else {
      SPLIT_TOKEN(1);
    } break;
  case '@':
    SPLIT_TOKEN(1);
    break;
  case '^':
    tok->op = OP_BIT_XOR;
    if (len > 1) {
      if (buf[1] == '^') {
        tok->op = OP_XOR;
        MAYBE_ASSIGN(2);
      } else {
        MAYBE_ASSIGN(1);
      }
    } break;
  case '|':
    tok->op = OP_BIT_OR;
    if (len > 1) {
      if (buf[1] == '|') {
        tok->op = OP_OR;
        MAYBE_ASSIGN(2);
      } else {
        MAYBE_ASSIGN(1);
      }
    } break;
  case '~':
    tok->op = OP_BIT_NOT;
    MAYBE_ASSIGN(1);
    break;
  }
}

void CompilationUnit::tokenize() {
  size_t line_number = 1;
  TokenizerState state = STATE_NULL;

  char* start = text;
  size_t start_line_number = 1;
  size_t start_index = 0;

  size_t i;
  for (i = 0; i < length; i++) {
    char c = text[i];
    if (c == '\n') line_number++;

    CharType type = get_type(c);

    switch (state) {
    case STATE_OP:
      switch (type) {
      case CHAR_PERIOD:
      case CHAR_PUNCT:
        break;
      default:
        END_TOKEN(TOKEN_OP);
        check_operator();
        break;
      } break;
    case STATE_BRACKET:
      END_TOKEN(TOKEN_BRACKET);
      break;
    case STATE_IDENT:
      switch (type) {
      case CHAR_IDENT:
      case CHAR_NUM:
        break;
      default:
        END_TOKEN(TOKEN_IDENT);
        check_keyword();
      } break;
    case STATE_NUM:
      switch (type) {
      case CHAR_IDENT:
      case CHAR_NUM:
      case CHAR_PERIOD:
        break;
      default:
        END_TOKEN(TOKEN_NUM);
      } break;
    case STATE_STR:
      switch (type) {
      case CHAR_ESC:
        state = STATE_STR_ESC;
        break;
      case CHAR_QUOTE:
        state = STATE_STR_END;
        break;
      }
      break;
    case STATE_STR_ESC:
      state = STATE_STR;
      break;
    case STATE_STR_END:
      END_TOKEN(TOKEN_STR);
      break;
    case STATE_SEMICOLON:
      END_TOKEN(TOKEN_SEMICOLON);
      break;
    case STATE_COMMA:
      END_TOKEN(TOKEN_COMMA);
      break;
    case STATE_COMMENT:
      if (c == '\n') state = STATE_NULL;
      break;
    }
    if (state == STATE_NULL) {
      switch (type) {
      case CHAR_PERIOD:
      case CHAR_PUNCT: BEGIN_TOKEN(STATE_OP); break;
      case CHAR_BRACKET: BEGIN_TOKEN(STATE_BRACKET); break;
      case CHAR_IDENT: BEGIN_TOKEN(STATE_IDENT); break;
      case CHAR_NUM: BEGIN_TOKEN(STATE_NUM); break;
      case CHAR_SEMICOLON: BEGIN_TOKEN(STATE_SEMICOLON); break;
      case CHAR_COMMA: BEGIN_TOKEN(STATE_COMMA); break;
      case CHAR_QUOTE: BEGIN_TOKEN(STATE_STR); break;
      case CHAR_COMMENT: state = STATE_COMMENT;
      }
    }
  }

  switch (state) {
  case STATE_NULL:
    status = UNIT_TOKEN;
    break;
  case STATE_OP: END_TOKEN(TOKEN_OP); break;
  case STATE_BRACKET: END_TOKEN(TOKEN_BRACKET); break;
  case STATE_IDENT:
    END_TOKEN(TOKEN_IDENT);
    check_keyword();
    break;
  case STATE_NUM: END_TOKEN(TOKEN_NUM); break;
  case STATE_STR:
  case STATE_STR_ESC:
    status = UNIT_ERROR;
    errors = true;
    break;
  case STATE_STR_END: END_TOKEN(TOKEN_STR); break;
  case STATE_SEMICOLON: END_TOKEN(TOKEN_SEMICOLON); break;
  case STATE_COMMA: END_TOKEN(TOKEN_COMMA); break;
  }
}

void CompilationUnit::dumpTokens() {
  for (auto& tok : tokens) {
    std::cout << "Line " << tok->line_number << " type ";
    switch (tok->type) {
    case TOKEN_STR: std::cout << "str "; break;
    case TOKEN_IDENT: std::cout << "ident "; break;
    case TOKEN_NUM: std::cout << "num "; break;
    case TOKEN_OP: std::cout << "op "; break;
    case TOKEN_BRACKET: std::cout << "bracket "; break;
    case TOKEN_SEMICOLON: std::cout << "semicolon "; break;
    case TOKEN_COMMA: std::cout << "comma "; break;
    default:
      std::cout << "WAAAT? (" << tok->type << ") ";
    }
    std::cout << tok->text << std::endl;
  }
}
