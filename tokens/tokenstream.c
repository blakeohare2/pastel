#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tokens.h"
#include "../io/disk.h"

enum {
	MODE_NONE = 0,
	MODE_STRING = 1,
	MODE_COMMENT = 2,
	MODE_WORD = 3,
};

void tokenizer_push_token(List* tokens, int* value, int column, int line)
{
	Token* token = new_token(value, column, line);
	list_add(tokens, token);
}

TokenStream* tokenize(FileContents* file_contents)
{
	int* raw_string = file_contents->text_data;
	int length = file_contents->text_length;
	int* file_name = file_contents->file_name;
	int i;	
	int token_start;
	int c;
	int c_t;
	int mode_type;
	int* quick_token;
	int mode = MODE_NONE;
	List* tokens = new_list();
	TokenStream* output;
	int handled;
	StringBuilderUtf8* token_builder = new_string_builder_utf8();
	int string_builder_length = 0;
	int string_builder_capacity = 100;
	int* lines = (int*) malloc(sizeof(int) * length);
	int* columns = (int*) malloc(sizeof(int) * length);
	int line = 0;
	int col = 0;
	
	for (i = 0; i < length; ++i)
	{
		lines[i] = line;
		columns[i] = col++;
		if (raw_string[i] == '\n')
		{
			line++;
			col = 0;
		}
	}
	
	i = 0;
	do
	{
		c = raw_string[i];
		switch (mode)
		{
			case MODE_NONE:
				handled = 0;
				switch (c)
				{
					case ' ':
					case '\r':
					case '\n':
					case '\t':
					case '\0':
						handled = 1;
						break;
						
					case '"':
					case '\'':
						mode = MODE_STRING;
						mode_type = c;
						token_start = i;
						handled = 1;
						string_builder_utf8_append_char(token_builder, c);
						break;
						
					case '/':
						c_t = raw_string[i + 1];
						if (c_t == '/' || c_t == '*')
						{
							mode = MODE_COMMENT;
							mode_type = c_t;
							i++;
							handled = 1;
						}
						break;
						
					default:
						switch ((c << 8) | raw_string[i + 1])
						{
							case ('+' << 8) | '+': // ++
							case ('-' << 8) | '-': // --
							case ('?' << 8) | '?': // ??
							case ('+' << 8) | '=': // +=
							case ('-' << 8) | '=': // -=
							case ('*' << 8) | '=': // *=
							case ('/' << 8) | '=': // /=
							case ('%' << 8) | '=': // %=
							case ('&' << 8) | '=': // &=
							case ('|' << 8) | '=': // |=
							case ('^' << 8) | '=': // ^=
							case ('=' << 8) | '=': // ==
							case ('!' << 8) | '=': // !=
							case ('<' << 8) | '=': // <=
							case ('>' << 8) | '=': // >=
								list_add(tokens, new_token(
									new_string_utf8_c2(c, raw_string[i + 1]),
									columns[i],
									lines[i]));
								i++;
								handled = 1;
								break;
							
							case ('&' << 8) | '&': // && and &&=
							case ('|' << 8) | '|': // || and ||=
							case ('*' << 8) | '*': // ** and **=
							case ('<' << 8) | '<': // << and <<=
							case ('>' << 8) | '>': // >> and >>=
								if (raw_string[i + 2] == '=')
								{
									list_add(tokens, new_token(
										new_string_utf8_c3(c, raw_string[i + 1], '='),
										columns[i],
										lines[i]));
									i += 2;
								}
								else
								{
									list_add(tokens, new_token(
										new_string_utf8_c2(c, raw_string[i + 1]),
										columns[i],
										lines[i]));
									i++;
								}
								handled = 1;
								break;
								
							default:
								break;
						}
						break;
				}
				
				if (handled == 0)
				{
					if ((c >= 'A' && c <= 'Z') ||
						(c >= 'a' && c <= 'z') ||
						(c >= '0' && c <= '9') ||
						c == '_' ||
						c == '$')
					{
						mode = MODE_WORD;
						string_builder_utf8_reset(token_builder);
						token_start = i;
						--i;
					}
					else
					{
						list_add(tokens, new_token(new_string_utf8_c1(c), columns[i], lines[i]));
					}
				}
				break;
				
			case MODE_STRING:
				string_builder_utf8_append_char(token_builder, c);
				
				if (c == mode_type)
				{
					mode = MODE_NONE;
					list_add(tokens, 
						new_token(string_builder_utf8_to_string(token_builder), columns[token_start], lines[token_start]));
					string_builder_utf8_reset(token_builder);
				}
				else if (c == '\\')
				{
					string_builder_utf8_append_char(token_builder, raw_string[i + 1]);
					++i;
				}
				break;
				
			case MODE_COMMENT:
				if (mode_type == '/')
				{
					if (c == '\n')
					{
						mode = MODE_NONE;
					}
				}
				else
				{
					if (c == '*' && raw_string[i + 1] == '/')
					{
						i++;
						mode = MODE_NONE;
					}
				}
				break;
				
			case MODE_WORD:
				if ((c >= 'A' && c <= 'Z') ||
					(c >= 'a' && c <= 'z') ||
					(c >= '0' && c <= '9') ||
					c == '_' ||
					c == '$')
				{
					string_builder_utf8_append_char(token_builder, c);
				}
				else
				{
					int* string_temp = string_builder_utf8_to_string(token_builder);
					list_add(tokens,
						new_token(
							string_temp,
							columns[token_start],
							lines[token_start]));
					--i;
					mode = MODE_NONE;
					string_builder_utf8_reset(token_builder);
				}
				break;
			default:
				break;
		}
		++i;
	} while (c != '\0');

	free(token_builder);
	free(lines);
	free(columns);
	
	output = (TokenStream*) malloc(sizeof(TokenStream));
	output->tokens = tokens;
	output->length = tokens->length;
	output->current_index = 0;
	output->filename = string_utf8_copy(file_contents->file_name);
	output->last = output->length > 0 ? (Token*) output->tokens->items[output->length - 1] : NULL;
	return output;
}

Token* tokens_pop(TokenStream* tokens)
{
	if (tokens->current_index == tokens->length)
	{
		return NULL;
	}
	return (Token*) tokens->tokens->items[tokens->current_index++];
}

Token* tokens_peek(TokenStream* tokens)
{
	if (tokens->current_index == tokens->length)
	{
		return NULL;
	}
	return (Token*) tokens->tokens->items[tokens->current_index];
}

int* tokens_peek_value(TokenStream* tokens)
{
	if (tokens->current_index == tokens->length)
	{
		return NULL;
	}
	return ((Token*) tokens->tokens->items[tokens->current_index])->value;
}

int tokens_has_more(TokenStream* tokens)
{
	return tokens->current_index < tokens->length;
}

void free_token_stream(TokenStream* tokens)
{
	int i;
	for (i = 0; i < tokens->length; ++i)
	{
		free_token((Token*) tokens->tokens->items[i]);
	}
	free_utf8_string(tokens->filename);
	free_list(tokens->tokens);
	free(tokens);
}

int tokens_is_next_chars(TokenStream* tokens, char* value)
{
	if (tokens->current_index < tokens->length)
	{
		return string_utf8_equals_chars(
			((Token*) tokens->tokens->items[tokens->current_index])->value,
			value);
	}
	return 0;
}

Token* tokens_pop_if_present_chars(TokenStream* tokens, char* value)
{
	if (tokens_is_next_chars(tokens, value))
	{
		return tokens_pop(tokens);
	}
	return NULL;
}
