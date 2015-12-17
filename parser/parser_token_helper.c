#include <stdlib.h>

#include "../tokens/tokens.h"
#include "../utils/utils.h"
#include "parser.h"

Token* tokens_pop_expected(ParserContext* context, TokenStream* tokens, char* value)
{
	Token* token = tokens_pop(tokens);
	if (token == NULL)
	{
		parser_context_set_error(context, tokens->last, new_heap_string("Unexpected EOF"));
		return NULL;
	}
	if (string_equals(value, token->value))
	{
		return token;
	}
	parser_context_set_error(context, token, string_concat5(
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
	
	if (token == NULL)
	{
		parser_context_set_error(context, tokens->last, new_heap_string("Unexpected EOF"));
		return NULL;
	}
	else
	{
		char* value = token->value;
		int is_identifier = 1;
		int i = 0;
		char c;
		while ((c = value[i++]) != '\0')
		{
			if ((c >= 'A' && c <= 'Z') ||
				(c >= 'a' && c <= 'z') ||
				(c >= '0' && c <= '9') ||
				c == '_' ||
				c == '$')
			{
				// This is fine.
			}
			else
			{
				is_identifier = 0;
				break;
			}
		}
		
		if (is_identifier && value[0] < '0' || value[0] > '9')
		{
			is_identifier = 0;
		}
		
		if (!is_identifier)
		{
			parser_context_set_error(context, token, string_concat4(stack_error_message, " Found: '", value, "'"));
			return NULL;
		}
		
		return token;
	}
}