#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

// LIST

List* new_list()
{
	List* output = (List*) malloc(sizeof(List));
	output->length = 0;
	output->capacity = 0;
	return output;
}

void list_ensure_capacity(List* list, int length)
{
	while (list->capacity < length)
	{
		if (list->length == 0)
		{
			list->items = (void**) malloc(sizeof(void*) * 8);
			list->capacity = 8;
		}
		else
		{
			int old_capacity = list->capacity;
			int new_capacity = (old_capacity + 1) * 2;
			void** old_items = list->items;
			void** new_items = (void**) malloc(sizeof(void*) * new_capacity);
			int i;
			// TODO: lookup memcpy or something like that, iirc
			for (i = 0; i < old_capacity; ++i)
			{
				new_items[i] = old_items[i];
			}
			list->capacity = new_capacity;
			list->items = new_items;
			free(old_items);
		}
	}
}

void list_add(List* list, void* item)
{
	list_ensure_capacity(list, list->length + 1);
	list->items[list->length++] = item;
}

// Frees the wrapped list, but NOT the items in it. That still has to be done manually since
// the type is not known.
void free_list(List* list)
{
	if (list != NULL)
	{
		free(list->items);
		free(list);
	}
}

// Integer list
ListInt* new_list_int()
{
	ListInt* output = (ListInt*) malloc(sizeof(ListInt));
	output->length = 0;
	output->capacity = 10;
	output->items = (int*) malloc(sizeof(int) * output->capacity);
	return output;
}

void list_int_add(ListInt* list, int num)
{
	if (list->length == list->capacity)
	{
		int new_capacity = list->capacity * 2 + 1;
		int* new_items = (int*) malloc(sizeof(int) * new_capacity);
		int* old_items = list->items;
		int i;
		memcpy(new_items, old_items, sizeof(int) * list->length);
		free(old_items);
		list->items = new_items;
		list->capacity = new_capacity;
	}
	
	list->items[list->length] = num;
	list->length += 1;
}

void free_list_int(ListInt* list)
{
	free(list->items);
	free(list);
}

// STRING BUILDER

StringBuilder* new_string_builder()
{
	return new_string_builder_with_capacity(20);
}

StringBuilder* new_string_builder_with_capacity(int capacity)
{
	StringBuilder* output = (StringBuilder*) malloc(sizeof(StringBuilder));
	if (capacity < 1)
	{
		capacity = 20;
	}
	output->length = 0;
	output->capacity = capacity;
	output->characters = (char*) malloc(sizeof(char) * capacity);
	return output;
}

void string_builder_ensure_capacity(StringBuilder* sb, int size)
{
	if (sb->length < size)
	{
		char* old_characters = sb->characters;
		char* new_characters;
		int new_length = sb->length * 2 + 1;
		while (new_length < size)
		{
			new_length *= 2;
		}
		new_characters = malloc(sizeof(char) * new_length);
		memcpy(new_characters, old_characters, sb->length * sizeof(char));
		free(old_characters);
		sb->characters = new_characters;
		sb->capacity = new_length;
	}
}

void string_builder_append_char(StringBuilder* sb, char c)
{
	if (sb->length == sb->capacity)
	{
		string_builder_ensure_capacity(sb, sb->length + 1);
	}
	
	sb->characters[sb->length++] = c;
}

void string_builder_append_chars(StringBuilder* sb, char* string)
{
	string_builder_append_chars_length(sb, string, strlen(string));
}

void string_builder_append_chars_length(StringBuilder* sb, char* characters, int length)
{
	int sb_length = sb->length;
	int new_length = sb_length + length;
	int i;
	string_builder_ensure_capacity(sb, new_length);
	for (i = 0; i < length; ++i)
	{
		sb->characters[sb_length + i] = characters[i];
	}
	sb->length = new_length;
}

char* string_builder_to_string(StringBuilder* sb)
{
	int length = sb->length;
	char* output = (char*) malloc(sizeof(char) * (length + 1));
	memcpy(output, sb->characters, length);
	output[length] = '\0';
	return output;
}

void string_builder_reset(StringBuilder* sb)
{
	sb->length = 0;
}

void free_string_builder(StringBuilder* sb)
{
	free(sb->characters);
	free(sb);
}

// STRING

char* new_string_c1(char c1)
{
	char* output = (char*) malloc(sizeof(char) * 2);
	output[0] = c1;
	output[1] = '\0';
	return output;
}

char* new_string_c2(char c1, char c2)
{
	char* output = (char*) malloc(sizeof(char) * 3);
	output[0] = c1;
	output[1] = c2;
	output[2] = '\0';
	return output;
}

char* new_string_c3(char c1, char c2, char c3)
{
	char* output = (char*) malloc(sizeof(char) * 4);
	output[0] = c1;
	output[1] = c2;
	output[2] = c3;
	output[3] = '\0';
	return output;
}

char* new_heap_string(char* other_string)
{
	int length = strlen(other_string) + 1;
	char* output = (char*) malloc(sizeof(char) * length);
	memcpy(output, other_string, sizeof(char) * length);
	return output;
}

int string_equals(char* s1, char* s2)
{
	int i = 0;
	char a, b;
	while (1)
	{
		a = s1[i];
		b = s2[i++];
		if (a == '\0' || b == '\0')
		{
			return a == b ? 1 : 0;
		}
		
		if (a != b)
		{
			return 0;
		}
	}
	return 0;
}

int string_starts_with(char* haystack, char* needle)
{
	int i = 0;
	while (needle[i] != '\0')
	{
		if (needle[i] != haystack[i])
		{
			return 0;
		}
		++i;
	}
	return 1;
}

int string_contains_char(char* haystack, char needle)
{
	int i = 0;
	while (haystack[i] != '\0')
	{
		if (haystack[i] == needle)
		{
			return 1;
		}
	}
	return 0;
}

int is_identifier(int* value)
{
	int i = 0;
	int length = string_utf8_length(value);
	int c;
	for (i = 0; i < length; ++i)
	{
		c = value[i];
		if ((c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			c == '_' || c == '$')
		{
			// this is fine.
		}
		else if (c >= '0' && c <= '9')
		{
			if (i == 0)
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	
	return 1;
}

int string_utf8_hash(int* value)
{
	int output = 0;
	int length = string_utf8_length(value);
	int i;
	for (i = 0; i < length; ++i)
	{
		output = (output << 19) - output + value[i];
	}
	return output;
}

int math_pow_int(int base, int exp)
{
	int output = 1;
	while (exp-- > 0)
	{
		output *= base;
	}
	return output;
}