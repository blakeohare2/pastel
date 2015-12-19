#ifndef _TOKENS_H_
#define _TOKENS_H_

#include "../utils/utils.h"
#include "../io/disk.h"

typedef struct Token {
	int* value;
	int line_index;
	int col_index;
	void* tokenstream;
} Token;

typedef struct TokenStream {
	List* tokens;
	Token* last;
	int length;
	int current_index;
	int* filename;
} TokenStream;

Token* new_token(int* value, int col, int line);
Token* tokens_pop(TokenStream* tokens);
Token* tokens_peek(TokenStream* tokens);
int* tokens_peek_value(TokenStream* tokens);
int tokens_has_more(TokenStream* tokens);
int tokens_is_next_chars(TokenStream* tokens, char* value);
Token* tokens_pop_if_present_chars(TokenStream* tokens, char* value);

void free_token_stream(TokenStream* tokens);
void free_token(Token* token);

TokenStream* tokenize(FileContents* file_contents);

#endif
