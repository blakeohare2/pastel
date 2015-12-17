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
char* string_concat2(char* s1, char* s2);
char* string_concat3(char* s1, char* s2, char* s3);
char* string_concat4(char* s1, char* s2, char* s3, char* s4);
char* string_concat5(char* s1, char* s2, char* s3, char* s4, char* s5);
char* string_concat6(char* s1, char* s2, char* s3, char* s4, char* s5, char* s6);

int is_identifier(char* value);
#endif
