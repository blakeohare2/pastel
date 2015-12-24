#include <stdio.h>
#include <stdlib.h>

#include "../utils/utils.h"
#include "../tokens/tokens.h"
#include "parser.h"

ParseNode* parse_for(ParserContext* context, TokenStream* tokens)
{
	if (context->verbose) printf("For Loop\n");
	
	ParseNode* node = new_node_for_loop();
	NodeForLoop* node_for = (NodeForLoop*) node->data;
	node->token = tokens_pop_expected_chars(context, tokens, "for");
	if (context->failed) { free_node(node); return NULL; }
	
	tokens_pop_expected_chars(context, tokens, "(");
	if (context->failed) { free_node(node); return NULL; }
	
	node_for->init = new_list();
	node_for->step = new_list();
	node_for->code = new_list();
	
	while (!tokens_pop_if_present_chars(tokens, ";"))
	{
		ParseNode* init_node;
		if (node_for->init->length > 0)
		{
			tokens_pop_expected_chars(context, tokens, ",");
			if (context->failed) { free_node(node); return NULL; }
		}
		init_node = parse_executable(context, tokens, 0 /* is root */, 1 /* simple */, 0 /* require semicolon */);
		list_add(node_for->init, init_node);
		if (context->failed) { free_node(node); return NULL; }
	}
	
	if (!tokens_pop_if_present_chars(tokens, ";"))
	{
		node_for->condition = parse_expression(context, tokens);
		if (context->failed) { free_node(node); return NULL; }
		
		tokens_pop_expected_chars(context, tokens, ";");
		if (context->failed) { free_node(node); return NULL; }
	}
	
	while (!tokens_pop_if_present_chars(tokens, ")"))
	{
		ParseNode* step_node;
		if (node_for->step->length > 0)
		{
			tokens_pop_expected_chars(context, tokens, ",");
			if (context->failed) { free_node(node); return NULL; }
		}
		step_node = parse_executable(context, tokens, 0 /* is root */, 1 /* simple */, 0 /* require semicolon */);
		list_add(node_for->step, step_node);
		if (context->failed) { free_node(node); return NULL; }
	}
	
	node_for->code = parse_code_block(context, tokens, 0);
	if (context->failed) { free_node(node); return NULL; }
	
	return node;
}

ParseNode* parse_function_definition(ParserContext* context, TokenStream* tokens)
{
	if (context->verbose) printf("Function Definition\n");
	
	ParseNode* node = new_node_function_definition();
	NodeFunctionDefinition* func_def = (NodeFunctionDefinition*) node->data;
	
	node->token = tokens_pop_expected_chars(context, tokens, "function");
	if (context->failed) { free_node(node); return NULL; }
	
	func_def->name_token = tokens_pop_identifier(context, tokens, "Invalid function name.");
	if (context->failed) { free_node(node); return NULL; }
	
	tokens_pop_expected_chars(context, tokens, "(");
	while (!tokens_pop_if_present_chars(tokens, ")"))
	{
		if (func_def->arg_names->length > 0)
		{
			tokens_pop_expected_chars(context, tokens, ",");
			if (context->failed) { free_node(node); return NULL; }
		}
		list_add(func_def->arg_names, tokens_pop_identifier(context, tokens, "Invalid parameter name."));
		if (context->failed) { free_node(node); return NULL; }
		
		if (tokens_pop_if_present_chars(tokens, "="))
		{
			list_add(func_def->arg_values, parse_expression(context, tokens));
			if (context->failed) { free_node(node); return NULL; }
		}
	}
	
	func_def->code = parse_code_block(context, tokens, 1 /* brackets required */);
	if (context->failed) { free_node(node); return NULL; }
	
	if (context->verbose) printf("Function definition done: %s\n", func_def->name_token->value);
	
	return node;
}

List* parse_code_block(ParserContext* context, TokenStream* tokens, int brackets_required)
{
	List* output = new_list();
	int has_brackets = brackets_required || tokens_is_next_chars(tokens, "{");
	if (has_brackets)
	{
		tokens_pop_expected_chars(context, tokens, "{");
		if (context->failed) { free_list(output); return NULL; }
		
		while (!tokens_pop_if_present_chars(tokens, "}"))
		{
			ParseNode* line = parse_executable(context, tokens, 0 /* root */, 0 /* simple */, 1 /* semicolon required */);
			list_add(output, line);
			if (context->failed) { free_node_list(output); return NULL; }
		}
	}
	else
	{
		ParseNode* line = parse_executable(context, tokens, 0 /* root */, 0 /* simple */, 1 /* semicolon required */);
		list_add(output, line);
		if (context->failed) { free_node_list(output); return NULL; }
	}
	return output;
}

ParseNode* parse_executable(
	ParserContext* context,
	TokenStream* tokens,
	int is_root,
	int only_allow_simple,
	int semicolon_present)
{
	if (context->verbose) printf("parse executable...\n");
	int* value = tokens_peek_value(tokens);
	ParseNode* exec = NULL;
	if (is_root)
	{
		if (string_utf8_equals_chars(value, "struct") && context->translate_platform != NULL)
		{
			exec = parse_struct(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "function") && is_root)
		{
			exec = parse_function_definition(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "class") && is_root)
		{
			exec = parse_class_definition(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "import") && is_root)
		{
			exec = parse_import(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "enum") && is_root)
		{
			exec = parse_enum(context, tokens);
		}
		
		if (context->failed) { free_node(exec); return NULL; }
		if (exec != NULL) return exec;
	}
	
	if (only_allow_simple == 0)
	{
		if (string_utf8_equals_chars(value, "for"))
		{
			exec = parse_for(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "while"))
		{
			exec = parse_while_loop(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "do"))
		{
			exec = parse_do_while_loop(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "switch"))
		{
			exec = parse_if(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "try"))
		{
			exec = parse_try(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "return"))
		{
			exec = parse_return(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "break"))
		{
			exec = parse_break(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "continue"))
		{
			exec = parse_continue(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "const"))
		{
			exec = parse_const(context, tokens);
		}
		else if (string_utf8_equals_chars(value, "constructor"))
		{
			exec = parse_constructor(context, tokens);
		}
	}
	
	if (context->failed) { free_node(exec); return NULL; }
	if (exec != NULL)
	{
		return exec;
	}
	
	if (exec == NULL)
	{
		ParseNode* expression = parse_expression(context, tokens);
		if (context->failed)
		{
			free_node(expression);
			return NULL;
		}
		else
		{
			Token* t = expression->token;
			int* next = tokens_peek_value(tokens);
			if (next != NULL)
			{
				int assignment_op = parser_context_get_assignment_op_enum(context, next);
				if (assignment_op != 0)
				{
					if (context->verbose) printf("Assignment start. target is %s\n", create_byte_string(expression->token->value));
					Token* assignment_token = tokens_pop(tokens);
					ParseNode* assignment_value_expression = parse_expression(context, tokens);
					if (context->failed)
					{
						free_node(expression);
						free_node(assignment_value_expression);
						return NULL;
					}
					else
					{
						ParseNode* assignment_node = new_node_assignment();
						NodeAssignment* assignment = (NodeAssignment*) assignment_node->data;
						Token* terken = expression->token;
						assignment_node->token = terken;
						assignment->target = expression;
						assignment->value = assignment_value_expression;
						assignment->op = assignment_op;
						assignment->op_token = assignment_token;
						expression = assignment_node;
					}
				}
			}
			
			if (semicolon_present)
			{
				tokens_pop_expected_chars(context, tokens, ";");
				if (context->failed)
				{
					free_node_assignment(expression);
					return NULL;
				}
			}
		}
		
		return expression;
	}
	
	parser_context_set_error(context, tokens_peek(tokens), string_concat_cic("Unexpected token: '", tokens_peek_value(tokens), "'"));
	return NULL;
}

ParseNode* parse_struct(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_struct");
	return NULL;
}

ParseNode* parse_constructor(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_constructor");
	return NULL;
}

ParseNode* parse_const(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_const");
	return NULL;
}

ParseNode* parse_continue(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_continue");
	return NULL;
}

ParseNode* parse_break(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_break");
	return NULL;
}

ParseNode* parse_try(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_try");
	return NULL;
}

ParseNode* parse_if(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_if");
	return NULL;
}

ParseNode* parse_do_while_loop(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_do_while_loop");
	return NULL;
}

ParseNode* parse_while_loop(ParserContext* context, TokenStream* tokens)
{
	ParseNode* node = new_node_while_loop();
	NodeWhileLoop* while_loop = (NodeWhileLoop*) node->data;
	node->token = tokens_pop_expected_chars(context, tokens, "while");
	if (context->failed) { free_node(node); return NULL; }
	
	tokens_pop_expected_chars(context, tokens, "(");
	if (context->failed) { free_node(node); return NULL; }
	
	while_loop->condition = parse_expression(context, tokens);
	if (context->failed) { free_node(node); return NULL; }
	
	tokens_pop_expected_chars(context, tokens, ")");
	if (context->failed) { free_node(node); return NULL; }
	
	while_loop->code = parse_code_block(context, tokens, 0);
	if (context->failed) { free_node(node); return NULL; }
	
	printf("While end\n");
	return node;
}

ParseNode* parse_enum(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_enum");
	return NULL;
}

ParseNode* parse_import(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_import");
	return NULL;
}

ParseNode* parse_class_definition(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_class_definition");
	return NULL;
}

ParseNode* parse_return(ParserContext* context, TokenStream* tokens)
{
	parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: parse_return");
	return NULL;
}

List* parse(ParserContext* context, TokenStream* tokens)
{
	List* output = new_list();
	int i = 0;
	while (tokens_has_more(tokens))
	{
		if (context->verbose) printf("Parse line #%d\n", ++i);
		ParseNode* line = parse_executable(
			context,
			tokens,
			1 /* root */,
			0 /* simple */,
			1 /* semicolons required */);
		list_add(output, line);
		if (context->failed) { free_node_list(output); return NULL; }
	}
	
	if (context->verbose) printf("parsing done\n");
	return output;
}
