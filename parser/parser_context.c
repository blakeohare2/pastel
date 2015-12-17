#include <stdio.h>
#include <stdlib.h>
#include "../utils/utils.h"
#include "../tokens/tokens.h"
#include "parser.h"

ParserContext* new_parser_context(int is_compile, char* platform_name, char* code_language)
{
	ParserContext* output = (ParserContext*) malloc(sizeof(ParserContext));
	MapStringInt* map;
	output->failed = 0;
	output->is_compile = is_compile;
	output->translate_platform = new_heap_string(platform_name);
	
	output->code_language = new_heap_string(code_language);
	
	output->error = NULL;
	output->error_token = NULL;
	
	// Build incremental assignment op to enum map
	map = new_map_string_int(20);
	
	map_string_int_put(map, "+=", OP_ADDITION);
	map_string_int_put(map, "-=", OP_SUBTRACTION);
	map_string_int_put(map, "*=", OP_MULTIPLICATION);
	map_string_int_put(map, "/=", OP_DIVISION);
	map_string_int_put(map, "%=", OP_MODULO);
	map_string_int_put(map, "**=", OP_EXPONENT);
	map_string_int_put(map, "&&=", OP_BOOLEAN_AND);
	map_string_int_put(map, "||=", OP_BOOLEAN_OR);
	map_string_int_put(map, "&=", OP_BITWISE_AND);
	map_string_int_put(map, "|=", OP_BITWISE_OR);
	map_string_int_put(map, "^=", OP_BITWISE_XOR);
	map_string_int_put(map, "<<=", OP_BIT_SHIFT_LEFT);
	map_string_int_put(map, ">>=", OP_BIT_SHIFT_RIGHT);
	map_string_int_put(map, "=", OP_EQUALS);
	output->op_assignment_lookup = map;
	
	return output;
}

void free_parser_context(ParserContext* parser_context)
{
	free(parser_context->translate_platform);
	free(parser_context->code_language);
	if (parser_context->error != NULL) free(parser_context->error);
	free_map_string_int(parser_context->op_assignment_lookup, 0);
	free(parser_context);
}

void parser_context_set_error(ParserContext* context, Token* token, char* heap_string)
{
	if (context->error != NULL)
	{
		free(context->error);
	}
	context->error_token = token;
	context->error = heap_string;
	context->failed = 1;
}

int parser_context_get_assignment_op_enum(ParserContext* context, char* assignment_token_value)
{
	return map_string_int_get(context->op_assignment_lookup, assignment_token_value, 0);
}