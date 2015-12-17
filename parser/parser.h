#ifndef _PARSER_H_
#define _PARSER_H_

#include "../utils/utils.h"
#include "../tokens/tokens.h"

typedef struct ParserContext {
	int is_compile;
	int failed;
	char* translate_platform;
	char* code_language;
	char* error; // ParserContext must free this.
	Token* error_token;
	MapStringInt* op_assignment_lookup;
} ParserContext;

ParserContext* new_parser_context(int is_compile, char* platform_name, char* code_language);
void parser_context_set_error(ParserContext* context, Token* token, char* heap_string);
void free_parser_context(ParserContext* parser_context);

enum OP {
	OP_ADDITION = 1,
	OP_SUBTRACTION = 2,
	OP_MULTIPLICATION = 3,
	OP_DIVISION = 4,
	OP_MODULO = 5,
	OP_EXPONENT = 6,
	OP_BOOLEAN_AND = 7,
	OP_BOOLEAN_OR = 8,
	OP_BITWISE_AND = 9,
	OP_BITWISE_OR = 10,
	OP_BITWISE_XOR = 11,
	OP_BIT_SHIFT_LEFT = 12,
	OP_BIT_SHIFT_RIGHT = 13,
	OP_EQUALS = 14,
	OP_NOT_EQUALS = 15,
	OP_LESS_THAN = 16,
	OP_LESS_THAN_OR_EQUAL = 17,
	OP_GREATER_THAN = 18,
	OP_GREATER_THAN_OR_EQUAL = 19
};

enum {
	NODE_ASSIGNMENT = 10,
	NODE_BINARY_OP = 20,
	NODE_FOR_LOOP = 30,
	NODE_FUNCTION_CALL = 40,
	NODE_FUNCTION_DEFINITION = 50,
	NODE_INTEGER_CONSTANT = 60,
	NODE_NULL_CONSTANT = 65,
	NODE_RETURN = 70,
	NODE_STRING_CONSTANT = 80,
	NODE_VARIABLE = 90
};

typedef struct ParseNode {
	int type;
	Token* token;
	void* data; // the "subclass"
} ParseNode;

typedef struct NodeAssignment {
	ParseNode* target;
	ParseNode* value;
	int op;
	Token* op_token;
} NodeAssignment;

typedef struct NodeBinaryOp {
	List* expressions; // List<ParseNode>
	List* op_tokens; // List<Token>
} NodeBinaryOp;

typedef struct NodeForLoop {
	Token* for_token;
	List* init; // List<ParseNode>
	ParseNode* condition; // (nullable) ParseNode
	List* step; // List<ParseNode>
	List* code; // List<ParseNode>
} NodeForLoop;

typedef struct NodeFunctionCall {
	ParseNode* root;
	List* arguments; // List<ParseNode>
} NodeFunctionCall;

typedef struct NodeFunctionDefinition {
	Token* name_token;
	List* arg_names; // List<Token>
	List* arg_values; // List<ParseNode>
	List* code; // List<ParseNode>
} NodeFunctionDefinition;

typedef struct NodeIntegerConstant {
	int value;
} NodeIntegerConstant;

typedef struct NodeReturn {
	ParseNode* value;
} NodeReturn;

typedef struct NodeStringConstant {
	char* value;
	int length; // because value can contain \0's in it.
} NodeStringConstant;

typedef struct NodeVariable {
	char* value;
} NodeVariable;

ParseNode* new_node_assignment();
ParseNode* new_node_binary_op();
ParseNode* new_node_for_loop();
ParseNode* new_node_function_call();
ParseNode* new_node_function_definition();
ParseNode* new_node_integer_constant();
ParseNode* new_node_null_constant();
ParseNode* new_node_return();
ParseNode* new_node_string_constant();
ParseNode* new_node_variable();


ParseNode* parse_executable(ParserContext* context, TokenStream* tokens, int is_root, int only_simple_allowed, int semicolon_required);
ParseNode* parse_expression(ParserContext* context, TokenStream* tokens);
List* parse_code_block(ParserContext* context, TokenStream* tokens, int brackets_required);

ParseNode* parse_assignment(ParserContext* context, TokenStream* tokens);
ParseNode* parse_binary_op(ParserContext* context, TokenStream* tokens);
ParseNode* parse_binary_op_addition(ParserContext* context, TokenStream* tokens);
ParseNode* parse_binary_op_multiplication(ParserContext* context, TokenStream* tokens);
ParseNode* parse_break(ParserContext* context, TokenStream* tokens);
ParseNode* parse_class_definition(ParserContext* context, TokenStream* tokens);
ParseNode* parse_const(ParserContext* context, TokenStream* tokens);
ParseNode* parse_constructor(ParserContext* context, TokenStream* tokens);
ParseNode* parse_continue(ParserContext* context, TokenStream* tokens);
ParseNode* parse_do_while_loop(ParserContext* context, TokenStream* tokens);
ParseNode* parse_enum(ParserContext* context, TokenStream* tokens);
ParseNode* parse_for_loop(ParserContext* context, TokenStream* tokens);
ParseNode* parse_function_call(ParserContext* context, TokenStream* tokens);
ParseNode* parse_function_definition(ParserContext* context, TokenStream* tokens);
ParseNode* parse_if(ParserContext* context, TokenStream* tokens);
ParseNode* parse_import(ParserContext* context, TokenStream* tokens);
ParseNode* parse_integer_constant(ParserContext* context, TokenStream* tokens);
ParseNode* parse_null_constant(ParserContext* context, TokenStream* tokens);
ParseNode* parse_parenthesis(ParserContext* context, TokenStream* tokens);
ParseNode* parse_return(ParserContext* context, TokenStream* tokens);
ParseNode* parse_string_constnat(ParserContext* context, TokenStream* tokens);
ParseNode* parse_struct(ParserContext* context, TokenStream* tokens);
ParseNode* parse_try(ParserContext* context, TokenStream* tokens);
ParseNode* parse_variable(ParserContext* context, TokenStream* tokens);
ParseNode* parse_while_loop(ParserContext* context, TokenStream* tokens);


void free_node(ParseNode* node);

void free_node_assignment(ParseNode* node);
void free_node_binary_op();
void free_node_for_loop(ParseNode* node);
void free_node_function_call(ParseNode* node);
void free_node_function_definition(ParseNode* node);
void free_node_integer_constant(ParseNode* node);
void free_node_null_constant(ParseNode* node);
void free_node_return(ParseNode* node);
void free_node_string_constant(ParseNode* node);
void free_node_variable(ParseNode* node);

// List of executable nodes.
List* parse(ParserContext* context, TokenStream* tokens);

void free_node_list(List* nodes);

Token* tokens_pop_expected(ParserContext* context, TokenStream* tokens, char* value);
Token* tokens_pop_identifier(ParserContext* context, TokenStream* tokens, char* stack_error_message);

char* parser_util_convert_string_token_to_value(ParserContext* context, Token* token, int* length_out);

#endif
