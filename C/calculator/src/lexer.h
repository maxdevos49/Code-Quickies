#ifndef __LEXER_H
#define __LEXER_H 1

#include <stdint.h>
#include <stdlib.h>

enum TokenType {
  TOKEN_OPEN_PAREN,
  TOKEN_CLOSE_PAREN,
  TOKEN_OPEN_BRACKET,
  TOKEN_CLOSE_BRACKET,
  TOKEN_OPEN_BRACE,
  TOKEN_CLOSE_BRACE,

  // Operators
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_SLASH,
  TOKEN_ASTERISK,
  TOKEN_CARET,
  TOKEN_EXCLAMATION,
  TOKEN_EQUALS,
  TOKEN_PERCENT,

  TOKEN_IDENTIFIER,

  TOKEN_NUMBER,

  TOKEN_LF,
  TOKEN_EOF
};

struct Token {
  enum TokenType type;
  uint32_t position;
  char *lexeme;
};

struct TokenArray {
  struct Token *data;
  size_t length;
  size_t capacity;
};

struct TokenArray *tokenize(size_t src_len, const char *src);
void free_token_array(struct TokenArray *tokens);
void print_token_array(struct TokenArray *tokens);

#endif
