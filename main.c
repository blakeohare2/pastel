#include <stdio.h>

#include "utils/utils.h"
#include "tokens/tokens.h"
#include "parser/parser.h"
#include "io/disk.h"

int main(int argc, char** argv)
{
	FileContents* sample_file = disk_read_file_text_char_path("start.cry");
	TokenStream* tokens = tokenize(sample_file);
	
	ParserContext* parser_context = new_parser_context(0, "python", "en");
	parser_context->verbose = 1;
	
	printf("parsing...\n");
	List* executables = parse(parser_context, tokens);
	printf("done parsing.\n");
	
	parser_context_print_error(parser_context);
	free_node_list(executables);
	free_token_stream(tokens);
	free_parser_context(parser_context);
	
	return 0;
}
