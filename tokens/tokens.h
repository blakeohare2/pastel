#ifndef _TOKENS_H_
#define _TOKENS_H_

#include "../utils/utils.h"

typedef struct Token {
	char* value;
	int line_index;
	int col_index;
	void* tokenstream;
} Token;

typedef struct TokenStream {
	List* tokens;
	Token* last;
	int length;
	int current_index;
	char* filename;
} TokenStream;

Token* new_token(char* value, int col, int line);
Token* tokens_pop(TokenStream* tokens);
Token* tokens_peek(TokenStream* tokens);
char* tokens_peek_value(TokenStream* tokens);
int tokens_has_more(TokenStream* tokens);
int tokens_is_next(TokenStream* tokens, char* value);
Token* tokens_pop_if_present(TokenStream* tokens, char* value);

void free_token_stream(TokenStream* tokens);
void free_token(Token* token);

TokenStream* tokenize(char* code, char* filename);

#endif
