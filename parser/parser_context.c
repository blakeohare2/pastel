#include <stdio.h>
#include <stdlib.h>
#include "../utils/utils.h"
#include "../tokens/tokens.h"
#include "parser.h"

ParserContext* new_parser_context(int is_compile, char* platform_name, char* code_language)
{
	ParserContext* output = (ParserContext*) malloc(sizeof(ParserContext));
	Map* map;
	output->failed = 0;
	output->is_compile = is_compile;
	output->translate_platform = new_heap_string(platform_name);
	output->verbose = 0;
	
	output->code_language = new_heap_string(code_language);
	
	output->error = NULL;
	output->error_token = NULL;
	
	// Build incremental assignment op to enum map
	map = new_map(20);
	
	map_put(map, chars_to_utf8("+="), OP_ADDITION);
	map_put(map, chars_to_utf8("-="), OP_SUBTRACTION);
	map_put(map, chars_to_utf8("*="), OP_MULTIPLICATION);
	map_put(map, chars_to_utf8("/="), OP_DIVISION);
	map_put(map, chars_to_utf8("%="), OP_MODULO);
	map_put(map, chars_to_utf8("**="), OP_EXPONENT);
	map_put(map, chars_to_utf8("&&="), OP_BOOLEAN_AND);
	map_put(map, chars_to_utf8("||="), OP_BOOLEAN_OR);
	map_put(map, chars_to_utf8("&="), OP_BITWISE_AND);
	map_put(map, chars_to_utf8("|="), OP_BITWISE_OR);
	map_put(map, chars_to_utf8("^="), OP_BITWISE_XOR);
	map_put(map, chars_to_utf8("<<="), OP_BIT_SHIFT_LEFT);
	map_put(map, chars_to_utf8(">>="), OP_BIT_SHIFT_RIGHT);
	map_put(map, chars_to_utf8("="), OP_EQUALS);
	output->op_assignment_lookup = map;
	
	return output;
}

void free_parser_context(ParserContext* parser_context)
{
	free(parser_context->translate_platform);
	free(parser_context->code_language);
	if (parser_context->error != NULL) free(parser_context->error);
	free_map(parser_context->op_assignment_lookup, 1);
	free(parser_context);
}

void parser_context_set_error_chars(ParserContext* context, Token* token, char* chars)
{
	parser_context_set_error(context, token, chars_to_utf8(chars));
}

void parser_context_set_error(ParserContext* context, Token* token, int* message)
{
	if (context->error != NULL)
	{
		free(context->error - 1);
	}
	context->error_token = token;
	context->error = message;
	context->failed = 1;
}

int parser_context_get_assignment_op_enum(ParserContext* context, int* assignment_token_value)
{
	return map_get(context->op_assignment_lookup, assignment_token_value, 0);
}

void parser_context_print_error(ParserContext* context)
{
	if (context->failed)
	{
		char* msg = create_byte_string(context->error);
		printf("Error encountered:\n%s\n", msg);
		free(msg);
		
		Token* token = context->error_token;
		if (token != NULL)
		{
			printf("Line: %d\n", token->line_index + 1);
			printf("Column: %d\n", token->col_index + 1);
		}
	}
}
