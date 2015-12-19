#ifndef _DISK_H_
#define _DISK_H_

typedef struct FileContents {
	int* file_name;
	char* binary_data;
	int binary_length;
	int* text_data; // UTF-8
	int text_length;
} FileContents;

FileContents* disk_read_file_text_char_path(char* path);
FileContents* disk_read_file_text_utf8_path(int* path);

FileContents* disk_read_file_binary_char_path(char* path);
FileContents* disk_read_file_binary_utf8_path(int* path);

void free_file_contents(FileContents* file_contents);

#endif
