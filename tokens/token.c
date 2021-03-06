#include <stdlib.h>
#include <stdio.h>

#include "tokens.h"
#include "../utils/utils.h"

Token* new_token(int* value, int col, int line)
{
	Token* output = (Token*) malloc(sizeof(Token));
	output->value = value;
	output->line_index = line;
	output->col_index = col;
	output->tokenstream = NULL; // Set by tokenizer
	return output;
}

void free_token(Token* token)
{
	free_utf8_string(token->value);
	free(token);
}
