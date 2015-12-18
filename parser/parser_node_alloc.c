#include <stdlib.h>

#include "../utils/utils.h"
#include "../tokens/tokens.h"
#include "parser.h"

ParseNode* new_node_generic(int type)
{
	ParseNode* node = (ParseNode*) malloc(sizeof(ParseNode));
	node->type = type;
	node->token = NULL;
	node->data = NULL;
	return node;
}

ParseNode* new_node_assignment()
{
	ParseNode* node = new_node_generic(NODE_ASSIGNMENT);
	NodeAssignment* data = (NodeAssignment*) malloc(sizeof(NodeAssignment));
	node->data = (void*) data;
	data->target = NULL;
	data->value = NULL;
	data->op = OP_EQUALS;
	return node;
}

ParseNode* new_node_binary_op()
{
	ParseNode* node = new_node_generic(NODE_BINARY_OP);
	NodeBinaryOp* data = (NodeBinaryOp*) malloc(sizeof(NodeBinaryOp));
	node->data = (void*) data;
	data->expressions = new_list();
	data->op_tokens = new_list();
	return node;
}

ParseNode* new_node_for_loop()
{
	ParseNode* node = new_node_generic(NODE_FOR_LOOP);
	NodeForLoop* for_loop = (NodeForLoop*) malloc(sizeof(NodeForLoop));
	node->data = for_loop;
	for_loop->for_token = NULL;
	for_loop->init = new_list();
	for_loop->condition = NULL;
	for_loop->step = new_list();
	for_loop->code = new_list();
	return node;
}

ParseNode* new_node_function_call()
{
	ParseNode* node = new_node_generic(NODE_FUNCTION_CALL);
	NodeFunctionCall* func_call = (NodeFunctionCall*) malloc(sizeof(NodeFunctionCall));
	node->data = func_call;
	func_call->root = NULL;
	func_call->open_paren_token = NULL;
	func_call->arguments = new_list();
	return node;
}

ParseNode* new_node_function_definition()
{
	ParseNode* node = new_node_generic(NODE_FUNCTION_DEFINITION);
	NodeFunctionDefinition* data = (NodeFunctionDefinition*) malloc(sizeof(NodeFunctionDefinition));
	node->data = data;
	data->name_token = NULL;
	data->arg_names = new_list();
	data->arg_values = new_list();
	data->code = NULL;
	return node;
}

ParseNode* new_node_string_constant()
{
	ParseNode* node = new_node_generic(NODE_STRING_CONSTANT);
	NodeStringConstant* sc = (NodeStringConstant*) malloc(sizeof(NodeStringConstant));
	node->data = sc;
	sc->value = NULL;
	sc->length = -1;
	return node;
}

ParseNode* new_node_variable()
{
	ParseNode* node = new_node_generic(NODE_VARIABLE);
	NodeVariable* v = (NodeVariable*) malloc(sizeof(NodeVariable));
	node->data = v;
	v->value = NULL;
	return node;
}

void free_node(ParseNode* node)
{
	if (node == NULL) return;
	switch (node->type)
	{
		case NODE_ASSIGNMENT: free_node_assignment(node); break;
		case NODE_BINARY_OP: free_node_binary_op(node); break;
		case NODE_FOR_LOOP: free_node_for_loop(node); break;
		case NODE_FUNCTION_CALL: free_node_function_call(node); break;
		case NODE_FUNCTION_DEFINITION: free_node_function_definition(node); break;
		case NODE_INTEGER_CONSTANT: free_node_integer_constant(node); break;
		case NODE_NULL_CONSTANT: free_node_null_constant(node); break;
		case NODE_RETURN: free_node_return(node); break;
		case NODE_STRING_CONSTANT: free_node_string_constant(node); break;
		case NODE_VARIABLE: free_node_variable(node); break;
		default: break;
	}
}

void free_node_assignment(ParseNode* node)
{
	NodeAssignment* asgn = (NodeAssignment*) node->data;
	free_node(asgn->target);
	free_node(asgn->value);
	free(asgn);
	free(node);
}

void free_node_binary_op(ParseNode* node)
{
	NodeBinaryOp* binop = (NodeBinaryOp*) node->data;
	free_node_list(binop->expressions);
	free_list(binop->op_tokens);
	free(binop);
	free(node);
}

void free_node_for_loop(ParseNode* node)
{
	NodeForLoop* for_loop = (NodeForLoop*) node->data;
	free_node_list(for_loop->init);
	free_node_list(for_loop->step);
	free_node_list(for_loop->code);
	free_node(for_loop->condition);
	free(for_loop);
	free(node);
}

void free_node_function_call(ParseNode* node)
{
	NodeFunctionCall* fc = (NodeFunctionCall*) node->data;
	free_node_list(fc->arguments);
	if (fc->root != NULL) free_node(fc->root);
	free(node);
}

void free_node_function_definition(ParseNode* node)
{
	NodeFunctionDefinition* func = (NodeFunctionDefinition*) node->data;
	free_list(func->arg_names);
	free_node_list(func->arg_values);
	free_node_list(func->code);
	free(func);
	free(node);
}

void free_node_integer_constant(ParseNode* node)
{
	NodeIntegerConstant* ic = (NodeIntegerConstant*) node->data;
	free(ic);
	free(node);
}

void free_node_list(List* nodes)
{
	if (nodes != NULL)
	{
		int i;
		for (i = nodes->length - 1; i >= 0; --i)
		{
			ParseNode* node = (ParseNode*) nodes->items[i];
			if (node != NULL)
			{
				free_node(node);
			}
		}
		free_list(nodes);
	}
}

void free_node_null_constant(ParseNode* node)
{
	// null constant has no data value.
	free(node);
}

void free_node_return(ParseNode* node)
{
	NodeReturn* r = (NodeReturn*) node->data;
	if (r->value != NULL) free_node(r->value);
	free(r);
	free(node);
}

void free_node_string_constant(ParseNode* node)
{
	NodeStringConstant* sc = (NodeStringConstant*) node->data;
	if (sc->value != NULL) free(sc->value);
	free(sc);
	free(node);
}

void free_node_variable(ParseNode* node)
{
	NodeVariable* v = (NodeVariable*) node->data;
	if (v->value != NULL) free(v->value);
	free(v);
	free(node);
}
