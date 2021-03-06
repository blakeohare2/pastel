#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

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

void List_ensure_capacity(List* list, int newLength)
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
		List_ensure_capacity(list, length + 1);
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
	int** output = (int**) wrapped_malloc(sizeof(int*) * 9);
	int str_0[] = {  };
	int str_1[] = { 65, 114, 103, 115, 58 };
	int str_2[] = { 32, 32, 32, 32 };
	int str_3[] = { 65, 108, 108, 32, 100, 111, 110, 101, 46 };
	int str_4[] = { 46 };
	int str_5[] = { 70, 105, 108, 101, 115, 32, 105, 110, 32, 116, 104, 105, 115, 32, 100, 105, 114, 101, 99, 116, 111, 114, 121, 58 };
	int str_6[] = { 32, 32, 32, 32, 62, 32 };
	int str_7[] = { 91, 68, 73, 82, 93, 32 };
	int str_8[] = { 109, 97, 105, 110, 46, 99 };
	output[0] = mem_inline_array(0, str_0);
	output[1] = mem_inline_array(5, str_1);
	output[2] = mem_inline_array(4, str_2);
	output[3] = mem_inline_array(9, str_3);
	output[4] = mem_inline_array(1, str_4);
	output[5] = mem_inline_array(24, str_5);
	output[6] = mem_inline_array(6, str_6);
	output[7] = mem_inline_array(6, str_7);
	output[8] = mem_inline_array(6, str_8);
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

char* create_byte_string(int* unicode_string)
{
	int utf8_length = unicode_string[-1];
	char* output = (char*) malloc(sizeof(char) * (utf8_length * 6 + 1));
	int length = 0;
	int i = 0;
	int utf8char;
	while (i < utf8_length)
	{
		utf8char = unicode_string[i++];
		if (utf8char < 0x80)
		{
			output[length++] = (char) utf8char;
		}
		else if (utf8char < 0x800)
		{
			output[length++] = (char) (0xC0 | (utf8char >> 6));
			output[length++] = (char) (0x80 | (0x3F & utf8char));
		}
		else if (utf8char < 0x10000)
		{
			output[length++] = (char) (0xE0 | (utf8char >> 12));
			output[length++] = (char) (0x80 | (0x3F & (utf8char >> 6)));
			output[length++] = (char) (0x80 | (0x3f & utf8char));
		}
		else if (utf8char < 0x200000)
		{
			output[length++] = (char) (0xF0 | (utf8char >> 18));
			output[length++] = (char) (0x80 | (0x3F & (utf8char >> 12)));
			output[length++] = (char) (0x80 | (0x3F & (utf8char >> 6)));
			output[length++] = (char) (0x80 | (0x3f & utf8char));
		}
		else if (utf8char <  0x4000000)
		{
			output[length++] = (char) (0xF8 | (utf8char >> 24));
			output[length++] = (char) (0x80 | (0x3F & (utf8char >> 18)));
			output[length++] = (char) (0x80 | (0x3F & (utf8char >> 12)));
			output[length++] = (char) (0x80 | (0x3F & (utf8char >> 6)));
			output[length++] = (char) (0x80 | (0x3f & utf8char));
		}
		else
		{
			output[length++] = (char) (0xFC | (utf8char >> 30));
			output[length++] = (char) (0x80 | (0x3F & (utf8char >> 24)));
			output[length++] = (char) (0x80 | (0x3F & (utf8char >> 18)));
			output[length++] = (char) (0x80 | (0x3F & (utf8char >> 12)));
			output[length++] = (char) (0x80 | (0x3F & (utf8char >> 6)));
			output[length++] = (char) (0x80 | (0x3f & utf8char));
		}
	}
	
	char* copied_output = (char*) malloc(sizeof(char) * (length + 1));
	memcpy(copied_output, output, length);
	free(output);
	copied_output[length] = '\0';
	return copied_output;
}
// These methods are designed to handle any slash-based filepath correctly on any system it may be compiled for.
// TODO: make the above statement not a blatant lie once I have a linux machine to implement/test that.

// Must only pass in heap strings that were generated in this file.
// Passing in a const string would be catastrophic.
// Although that shouldn't be difficult as all methods accept unicode strings which are temporarily converted to heap-based char strings.
void io_correct_path_separator(char* path, char old_char, char new_char)
{
	int i;
	for (i = 0; path[i] != '\0'; ++i)
	{
		if (path[i] == old_char)
		{
			path[i] = new_char;
		}
	}
}

List* SM_IO_list_dir(int* unicode_path)
{
	char* path = create_byte_string(unicode_path);

	// will have to ifdef/else/endif this and do magic in the make file.
	io_correct_path_separator(path, '/', '\\');

	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(path)) != NULL)
	{
		List* output = SM_List_new();
		while ((ent = readdir(dir)) != NULL)
		{
			char* filename = ent->d_name;
			if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0)
			{
				int* filename_unicode = unistring_from_text_chars(filename);
				SM_List_add(output, (int) filename_unicode);
			}
		}
		closedir(dir);
		free(path);
		return output;
	}
	free(path);
	return NULL;
}

int SM_IO_is_directory(int* unicode_path)
{
	char* path = create_byte_string(unicode_path);
	io_correct_path_separator(path, '/', '\\');
	struct stat statbuf;
	stat(path, &statbuf);
	free(path);
	if (S_ISDIR(statbuf.st_mode)) return 1;
	return 0;
}

int* SM_IO_read_all_text(int* unicode_path)
{
	char* path = create_byte_string(unicode_path);
	io_correct_path_separator(path, '/', '\\');
	
	FILE* fp = fopen(path, "r");
	int* final_output = NULL;
	if (fp != NULL)
	{
		char buffer[201];
		char* output = malloc(sizeof(char) * 200);
		int length = 0;
		int capacity = 200;
		while (!feof(fp))
		{
			int read_bytes = fread(buffer, sizeof(char), sizeof(char) * 200, fp);
			if (length + read_bytes > capacity)
			{
				int new_capacity = capacity;
				while (new_capacity < length + read_bytes)
				{
					new_capacity = new_capacity * 2 + 1;
				}
				char* new_output = (char*) malloc(sizeof(char) * new_capacity);
				memcpy(new_output, output, length);
				free(output);
				output = new_output;
				capacity = new_capacity;
			}
			memcpy(output + length, buffer, sizeof(char) * read_bytes);
			length += read_bytes;
		}
		fclose(fp);
		final_output = unistring_from_binary_chars(output, length);
		free(output);
	}
	free(path);
	return final_output;
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
	List* v_files = SM_IO_list_dir(get_string_constant(4));
	SM_System_println(get_string_constant(5));
	for (v_i = 0; (v_i < SM_List_length(v_files)); ++v_i)
	{
		SM_System_print(get_string_constant(6));
		if (SM_IO_is_directory(SM_List_get(v_files, v_i)))
		{
			SM_System_print(get_string_constant(7));
		}
		SM_System_println(SM_List_get(v_files, v_i));
	}
	int* v_main_c_contents = SM_IO_read_all_text(get_string_constant(8));
	SM_System_println(v_main_c_contents);
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
