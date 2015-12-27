#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int unistring_length(int* s)
{
	return s[-1];
}


typedef struct Token {
	int* value;
	int line;
	int col;
	int* filename;
} Token;

typedef struct StringBuilder {
	int capacity;
	int size;
	int* characters;
} StringBuilder;

typedef struct TokenStream {
	int current;
	int length;
	Token** tokens;
} TokenStream;

TokenStream* tokenize(int* filename, int* code)
{	int length = SM_UniString_length(v_code);
	int* lines = ((int*) allocate_array(sizeof(int * (v_length + 1))));
	int* columns = ((int*) allocate_array(sizeof(int * (v_length + 1))));
	List* tokens = cnstrOUTER_List();
	int line = 0;
	int col = 0;
	int i = 0;
	int c = 0;
	int c2 = 0;
	int c3 = 0;
	for (v_i = 0; (v_i <= v_length); ++v_i)
	{
		v_lines[v_i] = v_line;
		v_columns[v_i] = v_col;
		if (((v_i < v_length) && (v_code[v_i] == '\n')))
		{
			v_line++;
			v_col = 0;
		}
		else
		{
			v_col++;
		}
	}
	StringBuilder* token_builder = cnstrOUTER_StringBuilder();
	int token_start = 0;
	int modeType = 0;
	int mode = 2;
	int handled = 0;
	for (v_i = 0; (v_i <= v_length); ++v_i)
	{
		v_c = (v_i < v_length) ? v_code[v_i] : '\0';
		switch (v_mode)
		{
			case 2:
				v_handled = 0;
				switch (v_c)
				{
					case ' '':
					case '\n':
					case '\t':
					case '\r':
						v_handled = 1;
						break;

					case '/'':
						if ((v_i < (v_length - 1)))
						{
							v_c2 = v_code[(v_i + 1)];
							if (((v_c2 == '*'') || (v_c2 == '/'')))
							{
								v_mode = 1;
								v_modeType = v_c2;
								v_handled = 1;
								v_i++;
							}
						}
						break;

					case '"'':
					case '\'':
						v_token_start = v_i;
						method_StringBuilder_append_char_2(v_token_builder, v_c);
						v_mode = 3;
						v_modeType = v_c;
						break;

					default:
						if ((((v_c >= 'a'') && (v_c <= 'z'')) || ((v_c >= 'A'') && (v_c <= 'Z'')) || ((v_c >= '0'') && (v_c <= '9'')) || (v_c == '_'') || (v_c == '$'')))
						{
							v_token_start = v_i;
							method_StringBuilder_clear_4(v_token_builder);
							v_mode = 4;
							--v_i;
							v_handled = 1;
						}
						break;
				}
				if (!(v_handled))
				{
					Token* token = cnstrOUTER_Token();
					v_token->line = v_lines[v_i];
					v_token->col = v_columns[v_i];
					v_token->value = cnstrOUTER_UniString(v_c);
					v_token->filename = v_filename;
					SM_List_add(v_tokens, v_token);
				}
				break;

			case 1:
			case 3:
			case 4:
			default:
				break;
		}
	}
}


int main(int argc, char** argv)
{
	printf("Hello, World!\n");
}
