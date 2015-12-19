#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../utils/utils.h"
#include "disk.h"

FileContents* disk_read_file_text_char_path(char* path)
{
	FileContents* output = disk_read_file_binary_char_path(path);
	if (output == NULL) return NULL;
	int* utf8_data = create_utf8_string(output->binary_data, output->binary_length);
	output->text_data = utf8_data;
	output->text_length = string_utf8_length(utf8_data);
	
	return output;
}

FileContents* disk_read_file_text_utf8_path(int* path)
{
	int byte_length;
	char* char_path = create_byte_string_with_length(path, &byte_length);
	FileContents* output = disk_read_file_text_char_path(char_path);
	free(char_path);
	return output;
}

FileContents* disk_read_file_binary_utf8_path(int* path)
{
	int byte_length;
	char* char_path = create_byte_string_with_length(path, &byte_length);
	FileContents* output = disk_read_file_binary_char_path(char_path);
	free(char_path);
	return output;
}

FileContents* disk_read_file_binary_char_path(char* path)
{
	FILE* fp = fopen(path, "r");
	char buffer[200];
	StringBuilder* sb = new_string_builder();
	while (1)
	{
		int read_bytes = fread(buffer, sizeof(char), 200, fp);
		if (read_bytes > 0)
		{
			string_builder_append_chars_length(sb, buffer, read_bytes);
		}
		
		if (read_bytes != 200)
		{
			break;
		}
	}
	fclose(fp);
	
	char* bytes = string_builder_to_string(sb);
	
	FileContents* output = (FileContents*) malloc(sizeof(FileContents));
	output->binary_data = bytes;
	output->binary_length = sb->length;
	free_string_builder(sb);
	output->text_data = NULL;
	output->text_length = 0;
	
	return output;
}

void free_file_contents(FileContents* file_contents)
{
	if (file_contents->file_name != NULL) free(file_contents->file_name);
	if (file_contents->binary_data != NULL) free(file_contents->binary_data);
	if (file_contents->text_data != NULL) free(file_contents->text_data);
	free(file_contents);
}
