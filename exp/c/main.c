#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct MemTracker {
	int** allocations;
	int length;
	int capacity;
} MemTracker;

static MemTracker* global_mem_tracker;

void mem_expand_capacity(MemTracker* mt)
{
	int newCapacity = mt->capacity * 2 + 1;
	int** newAllocations = (int**) malloc(sizeof(int*) * newCapacity);
	memcpy(newAllocations, mt->allocations, mt->length * sizeof(int*));
	free(mt->allocations);
	mt->allocations = newAllocations;
	mt->capacity = newCapacity;
}

int* wrapped_malloc(int num_bytes)
{
	if (global_mem_tracker->length == global_mem_tracker->capacity) mem_expand_capacity(global_mem_tracker);
	int* data = malloc(num_bytes + sizeof(int));
	data[0] = global_mem_tracker->length;
	global_mem_tracker->allocations[global_mem_tracker->length++] = data;
	return data + 1;
}

int* wrapped_free(void* ptr)
{
	int* i_ptr = ((int*) ptr) - 1;
	int index = i_ptr[0];
	free(i_ptr);
	global_mem_tracker->length--;
	if (index != global_mem_tracker->length)
	{
		global_mem_tracker->allocations[index] = global_mem_tracker->allocations[global_mem_tracker->length];
	}
}

void mem_free_everything()
{
	MemTracker* mt = global_mem_tracker;
	int i;
	for (i = 0; i < mt->length; ++i)
	{
		free(mt->allocations[i]);
	}
	free(mt->allocations);
	free(mt);
}

int* mem_allocate_array(int member_size, int items)
{
	int* output = wrapped_malloc(member_size * items + sizeof(int));
	output[0] = items;
	return output + 1;
}

int array_length(void* array)
{
	return ((int*)array)[-1];
}
typedef struct List {
	int capacity;
	int length;
	int* items;
} List;

void ensure_capacity(List* list, int newLength)
{
	if (newLength > list->capacity)
	{
		int* newItems = malloc
	}
}

void list_add(void* item)
{

}

int is_unicode_bom_present(char* text)
{
	return text[0] == (char) 239 && text[1] == (char) 187 && text[2] == (char) 191;
}

int unistring_length(int* s)
{
	return s[-1];
}

int* unistring_from_text_chars(char* raw_chars)
{
	return unistring_from_binary_chars(raw_chars, strlen(raw_chars));
}

int* unistring_from_binary_chars(char* binary_data, int length)
{
	int i = is_unicode_bom_present(binary_data) ? 3 : 0;
	unsigned char c;
	int* output = malloc(sizeof(int) * length);
	int utf8_length = 0;
	int valid_char = -1;
	int char_size = 0;

	char temp[2];
	temp[1] = '\0';
	
	while (i < length)
	{
		utf8_length++;
		c = binary_data[i];
		if (c < 128)
		{
			output[i] = c;
			temp[0] = c;
		}
		else
		{
			valid_char = -1;
			temp[0] = c;
			if ((c & 0xC0) == 0xC0) // 2 byte marker
			{
				if (i < length - 1 && 
					(binary_data[i + 1] & 0x80) == 0x80)
				{
					char_size = 2;
					valid_char =
						((binary_data[i] & 0x1F) << 6) |
						(binary_data[i + 1] & 0x3F);
				}
			}
			else if ((c & 0xE0) == 0xE0) // 3 byte marker
			{
				if (i < length - 2 &&
					(binary_data[i + 1] & 0x80) == 0x80 &&
					(binary_data[i + 2] & 0x80) == 0x80)
				{
					char_size = 3;
					valid_char =
						((binary_data[i] & 0x0F) << 12) |
						((binary_data[i + 1] & 0x3F) << 6) |
						(binary_data[i + 2] & 0x3F);
				}
			}
			else if ((c & 0xF0) == 0xF0) // 4 byte marker
			{
				if (i < length - 3 &&
					(binary_data[i + 1] & 0x80) == 0x80 &&
					(binary_data[i + 2] & 0x80) == 0x80 &&
					(binary_data[i + 3] & 0x80) == 0x80)
				{
					char_size = 4;
					valid_char =
						((binary_data[i] & 0x07) << 18) |
						((binary_data[i + 1] & 0x3F) << 12) |
						((binary_data[i + 2] & 0x3F) << 6) |
						(binary_data[i + 3] & 0x3F);
				}
			}
			else if ((c & 0xF8) == 0xF8) // 5 byte marker
			{
				if (i < length - 4 &&
					(binary_data[i + 1] & 0x80) == 0x80 &&
					(binary_data[i + 2] & 0x80) == 0x80 &&
					(binary_data[i + 3] & 0x80) == 0x80 &&
					(binary_data[i + 4] & 0x80) == 0x80)
				{
					char_size = 5;
					valid_char =
						((binary_data[i] & 0x03) << 24) |
						((binary_data[i + 1] & 0x3F) << 18) |
						((binary_data[i + 2] & 0x3F) << 12) |
						((binary_data[i + 3] & 0x3F) << 6) |
						(binary_data[i + 4] & 0x3F);
				}
			}
			else if ((c & 0xFC) == 0xFC) // 6 byte marker
			{
				if (i < length - 5 &&
					(binary_data[i + 1] & 0x80) == 0x80 &&
					(binary_data[i + 2] & 0x80) == 0x80 &&
					(binary_data[i + 3] & 0x80) == 0x80 &&
					(binary_data[i + 4] & 0x80) == 0x80 &&
					(binary_data[i + 5] & 0x80) == 0x80)
				{
					char_size = 6;
					valid_char =
						((binary_data[i] & 0x01) << 30) |
						((binary_data[i + 1] & 0x3F) << 24) |
						((binary_data[i + 2] & 0x3F) << 18) |
						((binary_data[i + 3] & 0x3F) << 12) |
						((binary_data[i + 4] & 0x3F) << 6) |
						(binary_data[i + 5] & 0x3F);
				}
			}
			
			if (valid_char == -1)
			{
				output[i] = c;
			}
			else
			{
				output[i] = valid_char;
				i += char_size - 1;
			}
		}
		++i;
	}
	
	int* copied_output = wrapped_malloc(sizeof(int) * (utf8_length + 1));
	copied_output[0] = utf8_length;
	memcpy(copied_output + 1, output, sizeof(int) * utf8_length);
	free(output);
	return copied_output + 1;
}

int* unistring_from_single_char(int c)
{
	int* output = wrapped_malloc(sizeof(int) * 2);
	output[0] = 1;
	output[1] = c;
	return output + 1;
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

TokenStream* FUN_5_tokenize(int* v_filename, int* v_code);
StringBuilder* CONS_OUTER_1_StringBuilder(int v_capacity);
void CONS_INNER_1_StringBuilder(StringBuilder* _this, int v_capacity);
StringBuilder* CONS_OUTER_2_StringBuilder();
void CONS_INNER_2_StringBuilder(StringBuilder* _this);
Token* CONS_OUTER_3_Token();
void CONS_INNER_3_Token(Token* _this);
TokenStream* CONS_OUTER_4_TokenStream();
void CONS_INNER_4_TokenStream(TokenStream* _this);
void METHOD_1_ensureCapacity(StringBuilder* _this, int v_newLength);
void METHOD_2_append_char(StringBuilder* _this, int v_c);
void METHOD_3_append_string(StringBuilder* _this, int* v_str);
void METHOD_4_clear(StringBuilder* _this);


TokenStream* FUN_tokenize(int* filename, int* code)
{
	int v_length = SM_UniString_length(v_code);
	int* v_lines = ((int*) allocate_array(sizeof(int), (v_length + 1)));
	int* v_columns = ((int*) allocate_array(sizeof(int), (v_length + 1)));
	List* v_tokens = SM_List_new();
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
	StringBuilder* v_token_builder = CONS_OUTER_2_StringBuilder();
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
					case ' ':
					case '\n':
					case '\t':
					case '\r':
						v_handled = 1;
						break;

					case '/':
						if ((v_i < (v_length - 1)))
						{
							v_c2 = v_code[(v_i + 1)];
							if (((v_c2 == '*') || (v_c2 == '/')))
							{
								v_mode = 1;
								v_modeType = v_c2;
								v_handled = 1;
								v_i++;
							}
						}
						break;

					case '"':
					case '\'':
						v_token_start = v_i;
						method_StringBuilder_append_char_2(v_token_builder, v_c);
						v_mode = 3;
						v_modeType = v_c;
						break;

					default:
						if ((((v_c >= 'a') && (v_c <= 'z')) || ((v_c >= 'A') && (v_c <= 'Z')) || ((v_c >= '0') && (v_c <= '9')) || (v_c == '_') || (v_c == '$')))
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
					Token* v_token = CONS_OUTER_3_Token();
					v_token->line = v_lines[v_i];
					v_token->col = v_columns[v_i];
					v_token->value = SM_UniString_new_from_single_char(v_c);
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
StringBuilder* CONS_OUTER_1_StringBuilder(int capacity)
{	StringBuilder* _this = (StringBuilder*) malloc(sizeof(StringBuilder));
	CONS_INNER_StringBuilder(_this, capacity);
	return _this;
}

void CONS_INNER_StringBuilder(StringBuilder* _this, int capacity)
	_this->capacity = v_capacity;
	_this->size = 0;
	_this->characters = ((int*) allocate_array(sizeof(int), _this->capacity));
}

StringBuilder* CONS_OUTER_2_StringBuilder()
{	StringBuilder* _this = (StringBuilder*) malloc(sizeof(StringBuilder));
	CONS_INNER_StringBuilder(_this);
	return _this;
}

void CONS_INNER_StringBuilder(StringBuilder* _this)
	_this->capacity = 10;
	_this->size = 0;
	_this->characters = ((int*) allocate_array(sizeof(int), _this->capacity));
}

Token* CONS_OUTER_3_Token()
{	Token* _this = (Token*) malloc(sizeof(Token));
	CONS_INNER_Token(_this);
	return _this;
}

void CONS_INNER_Token(Token* _this)
}

TokenStream* CONS_OUTER_4_TokenStream()
{	TokenStream* _this = (TokenStream*) malloc(sizeof(TokenStream));
	CONS_INNER_TokenStream(_this);
	return _this;
}

void CONS_INNER_TokenStream(TokenStream* _this)
}


void METHOD_1_ensureCapacity(StringBuilder* _this, int newLength)
{
	int v_newCapacity = _this->capacity;
	while ((v_newCapacity < v_newLength))
	{
		v_newCapacity = ((v_newCapacity * 2) + 1);
	}
	int* v_newCharacters = ((int*) allocate_array(sizeof(int), v_newCapacity));
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
	global_mem_tracker = (MemTracker*) malloc(sizeof(MemTracker));
	global_mem_tracker->capacity = 1000;
	global_mem_tracker->size = 0;
	global_mem_tracker->allocations = NULL;

	initialzie_string_table();

	List* args = new_list(argc);
	int i;
	for (i = 0; i < argc; ++i)
	{
		list_add(args, unistring_from_char(argv[i]));
	}

	v_main(args);
	mem_free_everything();
}
