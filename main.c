#include <stdio.h>

#include "utils/utils.h"
#include "tokens/tokens.h"
#include "parser/parser.h"

int main(int argc, char** argv)
{
	int i;
	char* sample_code = 
		"function foo() {\n\t$print(\"Hello, World!\");\n}";
		//"1 2 3 4 5 6 7 8 9 10";
	
	TokenStream* tokens = tokenize(sample_code, new_heap_string("test.cry"));
	
	ParserContext* parser_context = new_parser_context(0, "python", "en");
	
	printf("parsing...\n");
	List* executables = parse(parser_context, tokens);
	printf("done parsing.\n");
	free_node_list(executables);
	free_token_stream(tokens);
	free_parser_context(parser_context);
	
	return 0;
}
