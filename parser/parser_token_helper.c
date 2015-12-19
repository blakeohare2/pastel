#include <stdlib.h>
#include <stdio.h>

#include "../tokens/tokens.h"
#include "../utils/utils.h"
#include "parser.h"

Token* tokens_pop_expected_chars(ParserContext* context, TokenStream* tokens, char* value)
{
	Token* token = tokens_pop(tokens);
	if (token == NULL)
	{
		parser_context_set_error_chars(context, tokens->last, "Unexpected EOF");
		return NULL;
	}
	
	if (string_utf8_equals_chars(token->value, value))
	{
		return token;
	}
	
	parser_context_set_error(context, token, string_concat_ciccc(
		"Unexpected token. Found '", 
		token->value,
		"', expected '", 
		value,
		"'"));
	
	return NULL;
}

Token* tokens_pop_identifier(ParserContext* context, TokenStream* tokens, char* stack_error_message)
{
	Token* token = tokens_pop(tokens);
	if (token != NULL)
	{
		if (is_identifier(token->value))
		{
			return token;
		}
	}
	
	parser_context_set_error(context, tokens->last, chars_to_utf8("Unexpected EOF"));
	return NULL;
}