#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "../tokens/tokens.h"
#include "parser.h"

char* parser_util_convert_string_token_to_value(ParserContext* context, Token* token, int* length_out)
{
	char* token_value = token->value;
	StringBuilder* sb = new_string_builder();
	char* output;
	int i;
	int length = strlen(token_value) - 1;
	int user_length = 0;
	char c;
	char* t;
	for (i = 1; i < length; ++i)
	{
		user_length++;
		c = token_value[i];
		if (c == '\\')
		{
			switch (token_value[++i])
			{
				case 'n': string_builder_append_char(sb, '\n'); break;
				case 'r': string_builder_append_char(sb, '\r'); break;
				case '0': string_builder_append_char(sb, '\0'); break; // hmm...
				case '"': string_builder_append_char(sb, '"'); break;
				case '\'': string_builder_append_char(sb, '\''); break;
				case 't': string_builder_append_char(sb, '\t'); break;
				case '\\': string_builder_append_char(sb, '\\'); break;
				default:
					t = new_string_c1(token_value[i]);
					parser_context_set_error(context, token, string_concat3("Unknown escape sequence: '\\", t, "'"));
					free(t);
					free_string_builder(sb);
					return NULL;
			}
		}
		else
		{
			string_builder_append_char(sb, c);
		}
	}
	
	output = string_builder_to_string(sb);
	free_string_builder(sb);
	*length_out = user_length;
	return output;
}