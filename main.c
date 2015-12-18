#include <stdio.h>

#include "utils/utils.h"
#include "tokens/tokens.h"
#include "parser/parser.h"

int main(int argc, char** argv)
{
	int i;
	char* sample_code = 
		"function foo() {\n\t$print(\"Hello, World!\");\n}";
	
	TokenStream* tokens = tokenize(sample_code, new_heap_string("test.cry"));
	
	ParserContext* parser_context = new_parser_context(0, "python", "en");
	parser_context->verbose = 0;
	
	printf("parsing...\n");
	List* executables = parse(parser_context, tokens);
	printf("done parsing.\n");
	if (parser_context->failed)
	{
		printf("Error encountered:\n%s\n", parser_context->error);
		Token* token = parser_context->error_token;
		if (token != NULL)
		{
			printf("Line: %d\n", token->line_index + 1);
			printf("Column: %d\n", token->col_index + 1);
		}
	}
	free_node_list(executables);
	free_token_stream(tokens);
	free_parser_context(parser_context);
	
	return 0;
}
