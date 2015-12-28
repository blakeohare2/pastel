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
	int* data = (int*) malloc(num_bytes + sizeof(int));
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

int* mem_inline_array(int size, int* values)
{
	int* output = wrapped_malloc(sizeof(int) * (size + 1));
	output[0] = size;
	int i;
	for (i = 0; i < size; ++i)
	{
		output[i + 1] = values[i];
	}
	return output + 1;
}

void SM_System_print(int* unistring)
{
	int length = unistring[-1];
	int i;
	char c;
	for (i = 0; i < length; ++i)
	{
		if (unistring[i] < 128)
		{
			printf("%c", (char) unistring[i]);
		}
		else
		{
			printf("?");
		}
	}
}

void SM_System_println(int* unistring)
{
	SM_System_print(unistring);
	printf("\n");
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
		int newCapacity = list->capacity;
		while (newCapacity < newLength)
		{
			newCapacity = newCapacity * 2 + 1;
		}

		int* newItems = wrapped_malloc(sizeof(int) * newCapacity);
		memcpy(newItems, list->items, sizeof(int) * list->length);
		wrapped_free(list->items);
		list->items = newItems;
		list->capacity = newCapacity;
	}
}

void SM_List_add(List* list, int item)
{
	int length = list->length;
	if (length == list->capacity)
	{
		ensure_capacity(list, length + 1);
	}
	list->items[length] = item;
	list->length++;
}

List* SM_List_new()
{
	List* output = (List*) wrapped_malloc(sizeof(List));
	output->capacity = 4;
	output->length = 0;
	output->items = malloc(sizeof(int) * 4);
	return output;
}

void* SM_List_get(List* list, int index)
{
	if (index < 0 || index >= list->length)
	{
		printf("Array index out of bounds: %d\n", index);
	}

	return (void*) list->items[index];
}

int SM_List_length(List* list)
{
	return list->length;
}

int** generate_string_constants()
{
	int** output = (int**) wrapped_malloc(sizeof(int*) * 4);
	int str_0[] = {  };
	int str_1[] = { 65, 114, 103, 115, 58 };
	int str_2[] = { 32, 32, 32, 32 };
	int str_3[] = { 65, 108, 108, 32, 100, 111, 110, 101, 46 };
	output[0] = mem_inline_array(0, str_0);
	output[1] = mem_inline_array(5, str_1);
	output[2] = mem_inline_array(4, str_2);
	output[3] = mem_inline_array(9, str_3);
	return output;
}

int is_unicode_bom_present(char* text)
{
	return text[0] == (char) 239 && text[1] == (char) 187 && text[2] == (char) 191;
}

int SM_UniString_length(int* s)
{
	return s[-1];
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

int* unistring_from_text_chars(char* raw_chars)
{
	return unistring_from_binary_chars(raw_chars, strlen(raw_chars));
}

int* SM_UniString_new_from_single_char(int c)
{
	int* output = wrapped_malloc(sizeof(int) * 2);
	output[0] = 1;
	output[1] = c;
	return output + 1;
}

int* get_string_constant(int id)
{
	static int** lookup = NULL;
	if (lookup == NULL)
	{
		lookup = generate_string_constants();
	}
	return lookup[id];
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

void v_main(List* v_args);
TokenStream* FUN_6_tokenize(int* v_filename, int* v_code);
StringBuilder* CONS_OUTER_1_StringBuilder(int v_capacity);
void CONS_INNER_1_StringBuilder(StringBuilder* _this, int v_capacity);
StringBuilder* CONS_OUTER_2_StringBuilder();
void CONS_INNER_2_StringBuilder(StringBuilder* _this);
Token* CONS_OUTER_3_Token();
void CONS_INNER_3_Token(Token* _this);
TokenStream* CONS_OUTER_4_TokenStream();
void CONS_INNER_4_TokenStream(TokenStream* _this);
void METHOD_2_ensureCapacity(StringBuilder* _this, int v_newLength);
void METHOD_3_append_char(StringBuilder* _this, int v_c);
void METHOD_4_append_string(StringBuilder* _this, int* v_str);
void METHOD_5_clear(StringBuilder* _this);


void v_main(List* v_args)
{
	int v_length = SM_List_length(v_args);
	int v_i = 0;
	SM_System_println(get_string_constant(1));
	for (v_i = 0; (v_i < v_length); ++v_i)
	{
		SM_System_print(get_string_constant(2));
		SM_System_println(SM_List_get(v_args, v_i));
	}
	SM_System_println(get_string_constant(3));
}

TokenStream* FUN_6_tokenize(int* v_filename, int* v_code)
{
	int v_length = SM_UniString_length(v_code);
	int* v_lines = ((int*) mem_allocate_array(sizeof(int), (v_length + 1)));
	int* v_columns = ((int*) mem_allocate_array(sizeof(int), (v_length + 1)));
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
						METHOD_3_append_char(v_token_builder, v_c);
						v_mode = 3;
						v_modeType = v_c;
						break;

					default:
						if ((((v_c >= 'a') && (v_c <= 'z')) || ((v_c >= 'A') && (v_c <= 'Z')) || ((v_c >= '0') && (v_c <= '9')) || (v_c == '_') || (v_c == '$')))
						{
							v_token_start = v_i;
							METHOD_5_clear(v_token_builder);
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
					SM_List_add(v_tokens, ((int)v_token));
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
StringBuilder* CONS_OUTER_1_StringBuilder(int v_capacity)
{	StringBuilder* _this = (StringBuilder*) malloc(sizeof(StringBuilder));
	CONS_INNER_1_StringBuilder(_this, v_capacity);
	return _this;
}

void CONS_INNER_1_StringBuilder(StringBuilder* _this, int v_capacity)
{
	_this->capacity = v_capacity;
	_this->size = 0;
	_this->characters = ((int*) mem_allocate_array(sizeof(int), _this->capacity));
}

StringBuilder* CONS_OUTER_2_StringBuilder()
{	StringBuilder* _this = (StringBuilder*) malloc(sizeof(StringBuilder));
	CONS_INNER_2_StringBuilder(_this);
	return _this;
}

void CONS_INNER_2_StringBuilder(StringBuilder* _this)
{
	_this->capacity = 10;
	_this->size = 0;
	_this->characters = ((int*) mem_allocate_array(sizeof(int), _this->capacity));
}

Token* CONS_OUTER_3_Token()
{	Token* _this = (Token*) malloc(sizeof(Token));
	CONS_INNER_3_Token(_this);
	return _this;
}

void CONS_INNER_3_Token(Token* _this)
{
}

TokenStream* CONS_OUTER_4_TokenStream()
{	TokenStream* _this = (TokenStream*) malloc(sizeof(TokenStream));
	CONS_INNER_4_TokenStream(_this);
	return _this;
}

void CONS_INNER_4_TokenStream(TokenStream* _this)
{
}


void METHOD_2_ensureCapacity(StringBuilder* _this, int v_newLength)
{
	int v_newCapacity = _this->capacity;
	while ((v_newCapacity < v_newLength))
	{
		v_newCapacity = ((v_newCapacity * 2) + 1);
	}
	int* v_newCharacters = ((int*) mem_allocate_array(sizeof(int), v_newCapacity));
	int v_i = 0;
	for (v_i = 0; (v_i < _this->size); ++v_i)
	{
		v_newCharacters[v_i] = _this->characters[v_i];
	}
	_this->characters = v_newCharacters;
	_this->capacity = v_newCapacity;
}

void METHOD_3_append_char(StringBuilder* _this, int v_c)
{
	if ((_this->size == _this->capacity))
	{
		METHOD_2_ensureCapacity(_this, (_this->capacity + 1));
	}
	_this->characters[_this->size++] = v_c;
}

void METHOD_4_append_string(StringBuilder* _this, int* v_str)
{
	int v_strLength = SM_UniString_length(v_str);
	int v_newLength = (_this->size + v_strLength);
	if ((v_newLength > _this->capacity))
	{
		METHOD_2_ensureCapacity(_this, v_newLength);
	}
	int v_i = 0;
	for (v_i = 0; (v_i < v_strLength); ++v_i)
	{
		_this->characters[(_this->size + v_i)] = v_str[v_i];
	}
	_this->size = v_newLength;
}

void METHOD_5_clear(StringBuilder* _this)
{
	_this->size = 0;
}



int main(int argc, char** argv)
{
	global_mem_tracker = (MemTracker*) malloc(sizeof(MemTracker));
	global_mem_tracker->capacity = 1000;
	global_mem_tracker->length = 0;
	global_mem_tracker->allocations = (int**) malloc(sizeof(int*) * global_mem_tracker->capacity);

	List* args = SM_List_new(argc);
	int i;
	for (i = 0; i < argc; ++i)
	{
		SM_List_add(args, (int) unistring_from_text_chars(argv[i]));
	}

	v_main(args);
	mem_free_everything();
}
