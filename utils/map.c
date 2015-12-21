#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

Map* new_map()
{
	return new_map_with_capacity(16);
}

Map* new_map_with_capacity(int capacity)
{
	int i;
	
	Map* output = (Map*) malloc(sizeof(Map));
	output->size = 0;
	if (capacity < 1) capacity = 8;
	output->capacity = capacity;
	output->buckets = (ListInt**) malloc(sizeof(ListInt*) * capacity);
	for (i = capacity - 1; i >= 0; --i)
	{
		output->buckets[i] = NULL;
	}
	return output;
}

void map_rehash(Map* map)
{
	// meh. I'll do this at some point.
}

// TODO: this is potentially prone to memory leaks in the event of an overwrite.
void map_put(Map* map, int* key, int value)
{
	int hash = string_utf8_hash(key);
	int bucket_id = hash % map->capacity;
	if (bucket_id < 0) bucket_id += map->capacity;
	ListInt* bucket = map->buckets[bucket_id];
	if (bucket == NULL)
	{
		bucket = new_list_int();
		map->buckets[bucket_id] = bucket;
	}
	
	int i;
	int bl = bucket->length;
	
	for (i = 0; i < bl; i += 2)
	{
		if (string_utf8_equals((int*) bucket->items[i], key))
		{
			bucket->items[i + 1] = value;
			return;
		}
	}
	
	list_int_add(bucket, (int) key);
	list_int_add(bucket, value);
	map->size++;
	
	if (map->size > map->capacity * 2)
	{
		map_rehash(map);
	}
}

int map_get(Map* map, int* key, int default_value)
{
	int hash_code = string_utf8_hash(key);
	List* bucket = (List*) map->buckets[hash_code % map->capacity];
	
	if (bucket != NULL)
	{
		int length = bucket->length;
		int i;
		for (i = 0; i < length; i += 2)
		{
			if (string_utf8_equals(key, (int*) bucket->items[i]))
			{
				return (int) bucket->items[i + 1];
			}
		}
	}
	return default_value;
}

int map_contains(Map* map, int* key)
{
	List* bucket = (List*) map->buckets[string_utf8_hash(key) % map->capacity];
	if (bucket != NULL)
	{
		int length = bucket->length;
		int i;
		for (i = length - 1; i >= 0; i -= 2)
		{
			if (string_utf8_equals(key, (int*) bucket->items[i]))
			{
				return 1;
			}
		}
	}
	return 0;
}

void free_map(Map* map, int free_keys)
{
	int i = map->capacity;
	while (--i >= 0)
	{
		List* bucket = (List*) map->buckets[i];
		if (bucket != NULL)
		{
			if (free_keys)
			{
				int j;
				for (j = 0; j < bucket->length; j += 2)
				{
					free_utf8_string((int*) bucket->items[j]);
				}
			}
			free(bucket);
		}
	}
	free(map);
}
