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

TokenStream* FUN_tokenize(int* filename, int* code)
{
	int v_length = SM_UniString_length(v_code);
	int* v_lines = ((int*) allocate_array(sizeof(int) * (v_length + 1)));
	int* v_columns = ((int*) allocate_array(sizeof(int) * (v_length + 1)));
	List* v_tokens = cnstrOUTER_List();
	int v_line = 0;
	int v_col = 0;
	int v_i = 0;
	int v_c = 0;
	int v_c2 = 0;
	int v_c3 = 0;
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
	StringBuilder* v_token_builder = cnstrOUTER_StringBuilder();
	int v_token_start = 0;
	int v_modeType = 0;
	int v_mode = 2;
	int v_handled = 0;
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
					Token* v_token = cnstrOUTER_Token();
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
StringBuilder* CONS_OUTER_StringBuilder(int capacity)
{	StringBuilder* _this = (StringBuilder*) malloc(sizeof(StringBuilder));
	CONS_INNER_StringBuilder(_this, capacity);
	return _this;
}

void CONS_INNER_StringBuilder(StringBuilder* _this, int capacity)
	_this->capacity = v_capacity;
	_this->size = 0;
	_this->characters = ((int*) allocate_array(sizeof(int) * _this->capacity));
}

StringBuilder* CONS_OUTER_StringBuilder()
{	StringBuilder* _this = (StringBuilder*) malloc(sizeof(StringBuilder));
	CONS_INNER_StringBuilder(_this);
	return _this;
}

void CONS_INNER_StringBuilder(StringBuilder* _this)
	_this->capacity = 10;
	_this->size = 0;
	_this->characters = ((int*) allocate_array(sizeof(int) * _this->capacity));
}


void METHOD_1_ensureCapacity(StringBuilder* _this, int newLength)
{
	int v_newCapacity = _this->capacity;
	while ((v_newCapacity < v_newLength))
	{
		v_newCapacity = ((v_newCapacity * 2) + 1);
	}
	int* v_newCharacters = ((int*) allocate_array(sizeof(int) * v_newCapacity));
	int v_i = 0;
	for (v_i = 0; (v_i < _this->size); ++v_i)
	{
		v_newCharacters[v_i] = _this->characters[v_i];
	}
	_this->characters = v_newCharacters;
	_this->capacity = v_newCapacity;
}

void METHOD_2_append_char(StringBuilder* _this, int c)
{
	if ((_this->size == _this->capacity))
	{
		method_StringBuilder_ensureCapacity_1(_this, (_this->capacity + 1));
	}
	_this->characters[v_this->size++] = v_c;
}

void METHOD_3_append_string(StringBuilder* _this, int* str)
{
	int v_strLength = SM_UniString_length(v_str);
	int v_newLength = (_this->size + v_strLength);
	if ((v_newLength > _this->capacity))
	{
		method_StringBuilder_ensureCapacity_1(_this, v_newLength);
	}
	int v_i = 0;
	for (v_i = 0; (v_i < v_strLength); ++v_i)
	{
		_this->characters[(_this->size + v_i)] = v_str[v_i];
	}
	_this->size = v_newLength;
}

void METHOD_4_clear(StringBuilder* _this, )
{
	_this->size = 0;
}


int main(int argc, char** argv)
{
	printf("Hello, World!\n");
}
