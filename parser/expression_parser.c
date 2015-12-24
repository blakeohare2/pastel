#include <stdlib.h>
#include <stdio.h>

#include "../utils/utils.h"
#include "../tokens/tokens.h"
#include "parser.h"

// All of these are private to this file.
ParseNode* parse_annnotation(ParserContext* context, TokenStream* tokens);
ParseNode* parse_ternary(ParserContext* context, TokenStream* tokens);
ParseNode* parse_null_coalescer(ParserContext* context, TokenStream* tokens);
ParseNode* parse_boolean_combination(ParserContext* context, TokenStream* tokens);
ParseNode* parse_bitwise_op(ParserContext* context, TokenStream* tokens);
ParseNode* parse_equality(ParserContext* context, TokenStream* tokens);
ParseNode* parse_inequality(ParserContext* context, TokenStream* tokens);
ParseNode* parse_bitshift(ParserContext* context, TokenStream* tokens);
ParseNode* parse_addition(ParserContext* context, TokenStream* tokens);
ParseNode* parse_multiplication(ParserContext* context, TokenStream* tokens);
ParseNode* parse_negate(ParserContext* context, TokenStream* tokens);
ParseNode* parse_exponents(ParserContext* context, TokenStream* tokens);
ParseNode* parse_increment(ParserContext* context, TokenStream* tokens);
ParseNode* parse_parenthesis(ParserContext* context, TokenStream* tokens);
ParseNode* parse_entity(ParserContext* context, TokenStream* tokens);
ParseNode* parse_entity_suffix(ParseNode* root, ParserContext* context, TokenStream* tokens);

//////////////////////////////////////

ParseNode* parse_expression(ParserContext* context, TokenStream* tokens)
{
	return parse_annnotation(context, tokens);
}

ParseNode* parse_annnotation(ParserContext* context, TokenStream* tokens)
{
	// TODO: this
	return parse_ternary(context, tokens);
}

ParseNode* parse_ternary(ParserContext* context, TokenStream* tokens)
{
	ParseNode* node = parse_null_coalescer(context, tokens);
	if (context->failed) { free_node(node); return NULL; }
	
	if (tokens_is_next_chars(tokens, "?"))
	{
		tokens_pop(tokens);
		
		ParseNode* true_result = parse_null_coalescer(context, tokens);
		if (context->failed) { free_node(node); return NULL; }
		
		tokens_pop_expected_chars(context, tokens, ":");
		if (context->failed) { free_node(node); return NULL; }
		
		ParseNode* false_result = parse_null_coalescer(context, tokens);
		if (context->failed) { free_node(node); return NULL; }
		
		ParseNode* ternary = new_node_ternary();
		ternary->token = node->token;
		
		NodeTernary* ternary_data = (NodeTernary*) ternary->data;
		ternary_data->true_expression = true_result;
		ternary_data->false_expression = false_result;
		ternary_data->condition = node;
		
		node = ternary;
	}
	
	return node;
}

ParseNode* parse_null_coalescer(ParserContext* context, TokenStream* tokens)
{
	ParseNode* node = parse_boolean_combination(context, tokens);
	if (context->failed) { free_node(node); return NULL; }
	
	if (tokens_is_next_chars(tokens, "??"))
	{
		tokens_pop(tokens);
		ParseNode* next = parse_null_coalescer(context, tokens);
		if (context->failed) { free_node(node); free_node(next); return NULL; }
		
		ParseNode* nc_node = new_node_null_coalescer();
		nc_node->token = node->token;
		NodeNullCoalescer* nc = (NodeNullCoalescer*) nc_node->data;
		nc->primary_expression = node;
		nc->secondary_expression = next;
		node = nc_node;
	}
	
	return node;
}

ParseNode* parse_boolean_combination(ParserContext* context, TokenStream* tokens)
{
	ParseNode* node = parse_bitwise_op(context, tokens);
	if (context->failed) { free_node(node); return NULL; }
	
	if (tokens_is_next_chars(tokens, "&&") || tokens_is_next_chars(tokens, "||"))
	{
		if (context->verbose) printf("Boolean combination\n");
		
		ParseNode* bc_node = new_node_boolean_combination();
		NodeBooleanCombination* bc = (NodeBooleanCombination*) bc_node->data;
		list_add(bc->expressions, node);
		node = bc_node;
		
		while (tokens_is_next_chars(tokens, "&&") || tokens_is_next_chars(tokens, "||"))
		{
			list_add(bc->op_tokens, tokens_pop(tokens));
			list_add(bc->expressions, parse_bitwise_op(context, tokens));
			if (context->failed) { free_node(node); return NULL; }
		}
	}
	
	return node;
}

ParseNode* parse_bitwise_op(ParserContext* context, TokenStream* tokens)
{
	ParseNode* node = parse_equality(context, tokens);
	if (context->failed) { free_node(node); return NULL; }
	
	if (tokens_is_next_chars(tokens, "&") ||
		tokens_is_next_chars(tokens, "|") ||
		tokens_is_next_chars(tokens, "^"))
	{
		if (context->verbose) printf("Op: bitwise operators\n");
		
		ParseNode* binop_node = new_node_binary_op();
		NodeBinaryOp* binop = (NodeBinaryOp*) node->data;
		list_add(binop->expressions, node);
		node = binop_node;
		
		while (tokens_is_next_chars(tokens, "&") ||
			tokens_is_next_chars(tokens, "|") ||
			tokens_is_next_chars(tokens, "^"))
		{
			list_add(binop->op_tokens, tokens_pop(tokens));
			list_add(binop->expressions, parse_equality(context, tokens));
			if (context->failed) { free_node(node); return NULL; }
		}
	}
	return node;
}

ParseNode* parse_equality(ParserContext* context, TokenStream* tokens)
{
	ParseNode* node = parse_inequality(context, tokens);
	if (context->failed) { free_node(node); return NULL; }
	
	if (tokens_is_next_chars(tokens, "==") ||
		tokens_is_next_chars(tokens, "!="))
	{
		if (context->verbose) printf("Op: equality operators\n");
		
		ParseNode* binop_node = new_node_binary_op();
		NodeBinaryOp* binop = (NodeBinaryOp*) node->data;
		list_add(binop->expressions, node);
		node = binop_node;
		list_add(binop->op_tokens, tokens_pop(tokens));
		list_add(binop->expressions, parse_inequality(context, tokens));
		if (context->failed) { free_node(node); return NULL; }
	}
	return node;
}

ParseNode* parse_inequality(ParserContext* context, TokenStream* tokens)
{
	ParseNode* node = parse_bitshift(context, tokens);
	if (context->failed) { free_node(node); return NULL; }
	
	if (tokens_is_next_chars(tokens, "<=") ||
		tokens_is_next_chars(tokens, ">=") ||
		tokens_is_next_chars(tokens, "<") ||
		tokens_is_next_chars(tokens, ">"))
	{
		if (context->verbose) printf("Op: inequality operators\n");
		
		ParseNode* binop_node = new_node_binary_op();
		NodeBinaryOp* binop = (NodeBinaryOp*) binop_node->data;
		list_add(binop->expressions, node);
		node = binop_node;
		list_add(binop->op_tokens, tokens_pop(tokens));
		list_add(binop->expressions, parse_bitshift(context, tokens));
		if (context->failed) { free_node(node); return NULL; }
	}
	return node;
}

ParseNode* parse_bitshift(ParserContext* context, TokenStream* tokens)
{
	ParseNode* node = parse_addition(context, tokens);
	if (context->failed) { free_node(node); return NULL; }
	
	if (tokens_is_next_chars(tokens, "<<") ||
		tokens_is_next_chars(tokens, ">>"))
	{
		if (context->verbose) printf("Op: bitshift\n");
		
		ParseNode* binop_node = new_node_binary_op();
		NodeBinaryOp* binop = (NodeBinaryOp*) node->data;
		list_add(binop->expressions, node);
		node = binop_node;
		
		while (tokens_is_next_chars(tokens, "<<") ||
			tokens_is_next_chars(tokens, ">>"))
		{
			list_add(binop->op_tokens, tokens_pop(tokens));
			list_add(binop->expressions, parse_addition(context, tokens));
			if (context->failed) { free_node(node); return NULL; }
		}
	}
	return node;
}

ParseNode* parse_addition(ParserContext* context, TokenStream* tokens)
{
	ParseNode* expr = parse_multiplication(context, tokens);
	if (context->failed) return NULL;
	
	if (tokens_is_next_chars(tokens, "+") ||
		tokens_is_next_chars(tokens, "-"))
	{
		if (context->verbose) printf("Op: addition/subtraction\n");
		
		ParseNode* node = new_node_binary_op();
		NodeBinaryOp* binop = (NodeBinaryOp*) node->data;
		list_add(binop->expressions, (void*) expr);
		while (tokens_is_next_chars(tokens, "+") ||
			tokens_is_next_chars(tokens, "-"))
		{
			list_add(binop->op_tokens, tokens_pop(tokens));
			list_add(binop->expressions, parse_multiplication(context, tokens));
			if (context->failed) { free_node(node); return NULL; }
		}
		expr = node;
	}
	
	return expr;
}

ParseNode* parse_multiplication(ParserContext* context, TokenStream* tokens)
{
	ParseNode* expr = parse_negate(context, tokens);
	if (context->failed) { free_node(expr); return NULL; }
	
	if (tokens_is_next_chars(tokens, "*") ||
		tokens_is_next_chars(tokens, "/") ||
		tokens_is_next_chars(tokens, "%"))
	{
		if (context->verbose) printf("Op: multiplication/division/modulo\n");
		
		ParseNode* node = new_node_binary_op();
		NodeBinaryOp* binop = (NodeBinaryOp*) node->data;
		list_add(binop->expressions, expr);
		node->token = expr->token;
		while (tokens_is_next_chars(tokens, "*") ||
			tokens_is_next_chars(tokens, "/") ||
			tokens_is_next_chars(tokens, "%"))
		{
			list_add(binop->op_tokens, tokens_pop(tokens));
			list_add(binop->expressions, parse_negate(context, tokens));
			if (context->failed) { free_node(node); return NULL; }
		}
		expr = node;
	}
	return expr;
}

ParseNode* parse_negate(ParserContext* context, TokenStream* tokens)
{
	if (tokens_is_next_chars(tokens, "-") || tokens_is_next_chars(tokens, "!"))
	{
		if (context->verbose) printf("Unary negation\n");
		
		Token* negate_token = tokens_pop(tokens);
		ParseNode* root = parse_negate(context, tokens);
		if (context->failed) { free_node(root); return NULL; }
		ParseNode* negate_node = new_node_negate();
		negate_node->token = negate_token;
		NodeNegate* negate = (NodeNegate*) negate_node->data;
		negate->expression = root;
		return negate_node;
	}
	return parse_exponents(context, tokens);
}

ParseNode* parse_exponents(ParserContext* context, TokenStream* tokens)
{
	ParseNode* expr = parse_increment(context, tokens);
	if (context->failed) { free_node(expr); return NULL; }
	
	if (tokens_is_next_chars(tokens, "**"))
	{
		if (context->verbose) printf("Op: exponents\n");
		
		ParseNode* node = new_node_binary_op();
		NodeBinaryOp* binop = (NodeBinaryOp*) node->data;
		list_add(binop->expressions, expr);
		while (tokens_is_next_chars(tokens, "**"))
		{
			list_add(binop->op_tokens, tokens_pop(tokens));
			list_add(binop->expressions, parse_increment(context, tokens));
			if (context->failed) { free_node(node); return NULL; }
		}
		expr = node;
	}
	return expr;
}

ParseNode* parse_increment(ParserContext* context, TokenStream* tokens)
{
	ParseNode* node = NULL;
	if (tokens_is_next_chars(tokens, "++") || tokens_is_next_chars(tokens, "--"))
	{
		Token* increment_token = tokens_pop(tokens);
		if (context->verbose) printf(string_utf8_equals_chars(increment_token->value, "++") ? "Prefix increment\n" : "Prefix decrement\n");
		ParseNode* root = parse_entity(context, tokens);
		if (context->failed) { free_node(root); return NULL; }
		
		node = new_node_increment();
		node->token = increment_token;
		NodeIncrement* inc = (NodeIncrement*) node->data;
		inc->increment_token = increment_token;
		inc->expression = root;
		inc->is_prefix = 1;
	}
	else
	{
		node = parse_entity(context, tokens);
		if (context->failed) { free_node(node); return NULL; }
		
		if (tokens_is_next_chars(tokens, "++") || tokens_is_next_chars(tokens, "--"))
		{
			ParseNode* inc_node = new_node_increment();
			inc_node->token = node->token;
			NodeIncrement* inc = (NodeIncrement*) inc_node->data;
			inc->expression = node;
			inc->increment_token = tokens_pop(tokens);
			inc->is_prefix = 0;
			node = inc_node;
			if (context->verbose) printf(string_utf8_equals_chars(inc->increment_token->value, "++") ? "Suffix increment\n" : "Suffix decrement\n");
		}
	}
	return node;
}

ParseNode* parse_parenthesis(ParserContext* context, TokenStream* tokens)
{
	if (context->verbose) printf("Parenthesis\n");
	
	tokens_pop_expected_chars(context, tokens, "(");
	if (context->failed) return NULL;
	
	ParseNode* node = parse_expression(context, tokens);
	if (context->failed) { free_node(node); return NULL; }
	
	tokens_pop_expected_chars(context, tokens, ")");
	if (context->failed) { free_node(node); return NULL; }
	
	return node;
}

ParseNode* parse_entity(ParserContext* context, TokenStream* tokens)
{
	if (tokens_is_next_chars(tokens, "("))
	{
		return parse_parenthesis(context, tokens);
	}
	
	int* next = tokens_peek_value(tokens);
	
	if (context->verbose) printf("Entity: %s\n", next);
	
	Token* token = tokens_pop(tokens);
	int c;
	ParseNode* expr = NULL;
	if (next == NULL)
	{
		parser_context_set_error_chars(context, tokens->last, "Unexpected EOF");
		return NULL;
	}
	c = next[0];
	
	if (c == '"' || c== '\'')
	{
		if (context->verbose) printf("String\n");
		
		int string_length;
		
		expr = new_node_string_constant();
		NodeStringConstant* string_const = (NodeStringConstant*) expr->data;
		expr->token = token;
		string_const->value = parser_util_convert_string_token_to_value(context, token);
	}
	else if (c >= '0' && c <= '9')
	{
		if (context->verbose) printf("Number\n");
		
		if (string_utf8_starts_with(next, "0x"))
		{
			parser_context_set_error_chars(context, token, "TODO: parse integer constant.");
			
		}
		else if (string_utf8_contains_char(next, '.'))
		{
			parser_context_set_error_chars(context, token, "TODO: parse float constant.");
		}
		else
		{
			int int_value = 0; // TODO: long long?
			int i;
			int token_length = string_utf8_length(token->value);
			for (i = 0; i < token_length; ++i)
			{
				c = next[i];
				if (c >= '0' && c <= '9')
				{
					int_value = int_value * 10 + (c - '0');
				}
				else
				{
					parser_context_set_error_chars(context, token, "Invalid variable name or number.");
					return NULL;
				}
			}
			expr = new_node_integer_constant();
			NodeIntegerConstant* ic = (NodeIntegerConstant*) expr->data;
			
			ic->value = int_value;
			expr->token = token;
			if (context->verbose) printf("Integer value: %d\n", int_value);
		}
	}
	else if (c == 'n' && string_utf8_equals_chars(next, "null"))
	{
		if (context->verbose) printf("Null\n");
		expr = new_node_null_constant();
		expr->token = token;
	}
	else if (c == 'n' && string_utf8_equals_chars(next, "new"))
	{
		if (context->verbose) printf("Constructor\n");
		
		parser_context_set_error_chars(context, token, "TODO: constructor invocation.");
	}
	else if (string_utf8_equals_chars(next, "true") || string_utf8_equals_chars(next, "false"))
	{
		if (context->verbose) printf("Boolean\n");
		
		parser_context_set_error_chars(context, token, "TODO: boolean constant.");
	}
	else if (
		(c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '$' || 
		c == '_')
	{
		if (context->verbose) printf("Word %s\n", create_byte_string(next));
		
		expr = new_node_variable();
		expr->token = token;
		NodeVariable* v = (NodeVariable*) expr->data;
		v->value = next;
	}
	else
	{
		parser_context_set_error(context, token, string_concat_cic("Unexpected token: '", next, "'"));
		return NULL;
	}
	
	if (context->failed) { free_node(expr); return NULL; }
	
	if (context->verbose) printf("entity done\n");
	
	return parse_entity_suffix(expr, context, tokens);
}

ParseNode* parse_entity_suffix(ParseNode* root, ParserContext* context, TokenStream* tokens)
{
	while (1)
	{
		if (tokens_is_next_chars(tokens, "("))
		{
			if (context->verbose) printf("Function call\n");
			
			ParseNode* func_call_node = new_node_function_call();
			NodeFunctionCall* func_call = (NodeFunctionCall*) func_call_node->data;
			func_call->root = root;
			func_call_node->token = func_call->root->token;
			func_call->open_paren_token = tokens_pop(tokens);
			root = func_call_node;
			while (!tokens_pop_if_present_chars(tokens, ")"))
			{
				if (func_call->arguments->length > 0)
				{
					tokens_pop_expected_chars(context, tokens, ",");
				}
				list_add(func_call->arguments, parse_expression(context, tokens));
				if (context->failed) { free_node(root); return NULL; }
			}
		}
		else if (tokens_is_next_chars(tokens, "."))
		{
			if (context->verbose) printf("Dot field\n");
		
			parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: dot field.");
			return NULL;
		}
		else if (tokens_is_next_chars(tokens, "["))
		{
			if (context->verbose) printf("Bracket index\n");
		
			parser_context_set_error_chars(context, tokens_peek(tokens), "TODO: bracket index/key.");
			return NULL;
		}
		else
		{
			break;
		}
		if (context->failed) { free_node(root); return NULL; }
	}
	
	return root;
}
