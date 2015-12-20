#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"

int is_unicode_bom_present(char* text)
{
	if (text[0] == '\0' || text[0] != (char) 239) return 0;
	if (text[1] == '\0' || text[1] != (char) 187) return 0;
	if (text[2] == '\0' || text[2] != (char) 191) return 0;
	return 1;
}

int* chars_to_utf8(char* binary_data)
{
	int length = strlen(binary_data);
	return create_utf8_string(binary_data, length);
}

int* create_utf8_string(char* binary_data, int length)
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
	
	int* copied_output = malloc(sizeof(int) * (utf8_length + 1));
	copied_output[0] = utf8_length;
	memcpy(copied_output + 1, output, sizeof(int) * utf8_length);
	free(output);
	return copied_output + 1;
}

char* create_byte_string(int* utf8_string)
{
	int ignored = 0;
	return create_byte_string_with_length(utf8_string, &ignored);
}

char* create_byte_string_with_length(int* utf8_string, int* length_out)
{
	int utf8_length = utf8_string[-1];
	char* output = (char*) malloc(sizeof(char) * (utf8_length * 6 + 1));
	int length = 0;
	int i = 0;
	int utf8char;
	while (i < utf8_length)
	{
		utf8char = utf8_string[i++];
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
	*length_out = length;
	memcpy(copied_output, output, length);
	free(output);
	copied_output[length] = '\0';
	return copied_output;
}

StringBuilderUtf8* new_string_builder_utf8()
{
	return new_string_builder_utf8_with_capacity(15);
}

StringBuilderUtf8* new_string_builder_utf8_with_capacity(int capacity)
{
	StringBuilderUtf8* sb = (StringBuilderUtf8*) malloc(sizeof(StringBuilderUtf8));
	if (capacity < 8) capacity = 8;
	sb->length = 0;
	sb->capacity = capacity;
	sb->characters = malloc(sizeof(int) * capacity);
	return sb;
}

void ensure_capacity(StringBuilderUtf8* sb, int capacity)
{
	if (capacity <= sb->capacity) return;
	int new_capacity = capacity;
	while (new_capacity < capacity)
	{
		new_capacity = new_capacity * 2 + 1;
	}
	int* new_chars = malloc(sizeof(int) * new_capacity);
	memcpy(new_chars, sb->characters, sizeof(int) * sb->length);
	free(sb->characters);
	sb->characters = new_chars;
	sb->capacity = new_capacity;
}

void string_builder_utf8_append_char(StringBuilderUtf8* sb, int c)
{
	ensure_capacity(sb, sb->length + 1);
	sb->characters[sb->length++] = c;
}

void string_builder_utf8_append_chars(StringBuilderUtf8* sb, int* characters)
{
	int length = characters[-1];
	ensure_capacity(sb, sb->length + length);
	int i = -1;
	while (++i < length)
	{
		sb->characters[sb->length++] = characters[i];
	}
}

void string_builder_utf8_append_char_ptr(StringBuilderUtf8* sb, char* chars)
{
	while (chars[0] != '\0')
	{
		string_builder_utf8_append_char(sb, chars[0]);
		chars += 1;
	}
}

int* string_builder_utf8_to_string(StringBuilderUtf8* sb)
{
	int length = sb->length;
	int* output = malloc(sizeof(int) * (length + 1));
	output[0] = length;
	output = output + 1;
	memcpy(output, sb->characters, sizeof(int) * length);
	return output;
}

void string_builder_utf8_reset(StringBuilderUtf8* sb)
{
	sb->length = 0;
}

void free_string_builder_utf8(StringBuilderUtf8* sb)
{
	free(sb->characters);
	free(sb);
}

void free_utf8_string(int* value)
{
	free(value - 1);
}

// All UTF8 strings allocated have an extra int before the pointer indicating the string length.
int string_utf8_length(int* str)
{
	return str[-1];
}

int string_utf8_equals(int* s1, int* s2)
{
	int* len1ptr = s1 - 1;
	int* len2ptr = s2 - 1;
	int length = len1ptr[0];
	if (length != len2ptr[0]) return 0;
	int i;
	for (i = 0; i < length; ++i)
	{
		if (s1[i] != s2[i])
		{
			return 0;
		}
	}
	return 1;
}

// Checks to see if the UTF8 string is equal to the ASCII string.
// Does NOT directly compare binary data. 
int string_utf8_equals_chars(int* str, char* chars)
{
	int length = str[-1];
	int i;
	char c;
	for (i = 0; i < length; ++i)
	{
		c = chars[i];
		if (c == '\0') return 0;
		if (str[i] != c)
		{
			return 0;
		}
	}
	return chars[length] == '\0';
}

int* new_string_utf8_c1(int c1)
{
	int* output = malloc(sizeof(int) * 2);
	output[0] = 1;
	output[1] = c1;
	return output;
}

int* new_string_utf8_c2(int c1, int c2)
{
	int* output = malloc(sizeof(int) * 3);
	output[0] = 2;
	output[1] = c1;
	output[2] = c2;
	return output;
}

int* new_string_utf8_c3(int c1, int c2, int c3)
{
	int* output = malloc(sizeof(int) * 4);
	output[0] = 3;
	output[1] = c1;
	output[2] = c2;
	output[3] = c3;
	return output;
}

int* new_heap_string_utf8(int* stack_string)
{
	int length = stack_string[-1];
	int* output = malloc(sizeof(int) * (length + 1));
	memcpy(output, stack_string - 1, sizeof(int) * (length + 1));
	return output + 1;
}

int* string_concat_cic(char* c1, int* s2, char* c3)
{
	return string_concat_cicic(c1, s2, c3, NULL, NULL);
}

int* string_concat_cicic(char* c1, int* s2, char* c3, int* s4, char* c5)
{
	StringBuilderUtf8* sb = new_string_builder_utf8();
	string_builder_utf8_append_char_ptr(sb, c1);
	string_builder_utf8_append_chars(sb, s2);
	string_builder_utf8_append_char_ptr(sb, c3);
	if (s4 != NULL) string_builder_utf8_append_chars(sb, s4);
	if (c5 != NULL) string_builder_utf8_append_char_ptr(sb, c5);
	int* output = string_builder_utf8_to_string(sb);
	free_string_builder_utf8(sb);
	return output;
}

int* string_concat_ciccc(char* c1, int* s2, char* c3, char* c4, char* c5)
{
	StringBuilderUtf8* sb = new_string_builder_utf8();
	string_builder_utf8_append_char_ptr(sb, c1);
	string_builder_utf8_append_chars(sb, s2);
	string_builder_utf8_append_char_ptr(sb, c3);
	if (c4 != NULL) string_builder_utf8_append_char_ptr(sb, c4);
	if (c5 != NULL) string_builder_utf8_append_char_ptr(sb, c5);
	int* output = string_builder_utf8_to_string(sb);
	free_string_builder_utf8(sb);
	return output;
}

int* string_utf8_copy(int* original)
{
	int length = string_utf8_length(original);
	int* output = malloc(sizeof(int) * (length + 1));
	memcpy(output + 1, original, sizeof(int) * length);
	output[0] = length;
	return output + 1;
}

int string_utf8_starts_with(int* haystack, char* needle)
{
	int length = string_utf8_length(haystack);
	int i;
	for (i = 0; i < length && needle[i] != '\0'; ++i)
	{
		if (haystack[i] != needle[i]) return 0;
	}
	return 1;
}

int string_utf8_contains_char(int* haystack, char needle)
{
	int length = string_utf8_length(haystack);
	int i = length - 1;
	while (i >= 0)
	{
		if (haystack[i--] == needle) return 1;
	}
	return 0;
}

void print_string_utf8(int* str)
{
	char text[2];
	text[1] = '\0';
	int i;
	int t;
	int length = string_utf8_length(str);
	for (i = 0; i < length; ++i)
	{
		t = str[i];
		text[0] = t < 128 ? t : '?';
		printf("%s", text);
	}
}

void println_string_utf8(int* str)
{
	print_string_utf8(str);
	printf("\n");
}
