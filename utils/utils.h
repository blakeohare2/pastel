#ifndef _UTILS_H_
#define _UTILS_H_

typedef struct List {
	int length;
	int capacity;
	void** items;
} List;

List* new_list();
void list_add(List* list, void* item);
void free_list(List* list);

typedef struct ListInt {
	int length;
	int capacity;
	int* items;
} ListInt;

ListInt* new_list_int();
void list_int_add(ListInt* list, int num);
void free_list_int(ListInt* list);

typedef struct StringBuilderUtf8 {
	int length;
	int capacity;
	int* characters;
} StringBuilderUtf8;

StringBuilderUtf8* new_string_builder_utf8();
StringBuilderUtf8* new_string_builder_utf8_with_capacity(int capacity);
void string_builder_utf8_append_char(StringBuilderUtf8* sb, int c);
void string_builder_utf8_append_chars(StringBuilderUtf8* sb, int* characters);
int* string_builder_utf8_to_string(StringBuilderUtf8* sb);
void string_builder_utf8_reset(StringBuilderUtf8* sb);
void free_string_builder_utf8(StringBuilderUtf8* sb);

typedef struct StringBuilder {
	int length;
	int capacity;
	char* characters;
} StringBuilder;

StringBuilder* new_string_builder();
StringBuilder* new_string_builder_with_capacity(int capacity);
void string_builder_append_char(StringBuilder* sb, char c);
void string_builder_append_chars(StringBuilder* sb, char* characters);
void string_builder_append_chars_length(StringBuilder* sb, char* characters, int length);
char* string_builder_to_string(StringBuilder* sb);
void string_builder_reset(StringBuilder* sb);
void free_string_builder(StringBuilder* sb);

char* new_string_c1(char c1);
char* new_string_c2(char c1, char c2);
char* new_string_c3(char c1, char c2, char c3);
char* new_heap_string(char* stack_string);

int* new_string_utf8_c1(int c1);
int* new_string_utf8_c2(int c1, int c2);
int* new_string_utf8_c3(int c1, int c2, int c3);
int* new_heap_string_utf8(int* stack_string);

typedef struct MapStringInt {
	int size;
	int capacity;
	ListInt** buckets; // List<int>[]: individual bucket lists have even length. 2n is the char*, 2n + 1 is the int value. cast.
} MapStringInt;

MapStringInt* new_map_string_int();
MapStringInt* new_map_string_int_with_capacity(int capacity);
void map_string_int_put(MapStringInt* map, char* key, int value);
int map_string_int_get(MapStringInt* map, char* key, int default_value);
void* map_string_ptr_get(MapStringInt* map, char* key);
int map_string_int_contains(MapStringInt* map, char* key);
void free_map_string_int(MapStringInt* map, int free_keys);

int string_equals(char* s1, char* s2);
int string_starts_with(char* haystack, char* needle);
int string_contains_char(char* haystack, char needle);

int* string_concat_cic(char* c1, int* s2, char* c3);
int* string_concat_cicic(char* c1, int* s2, char* c3, int* s4, char* c5);
int* string_concat_ciccc(char* c1, int* s2, char* c3, char* c4, char* c5);

int is_identifier(int* value);

int* create_utf8_string(char* binary_data, int binary_length);
char* create_byte_string(int* utf8_string);
char* create_byte_string_with_length(int* utf8_string, int* length_out);
int* string_utf8_copy(int* original);
void free_utf8_string(int* value);

int string_utf8_equals(int* s1, int* s2);
int string_utf8_equals_chars(int* str, char* chars);
int string_utf8_starts_with(int* haystack, char* needle);
int string_utf8_contains_char(int* haystack, char needle);

int string_utf8_length(int* str);
int* chars_to_utf8(char* binary_data);


void print_string_utf8(int* str);
void println_string_utf8(int* str);

#endif
