#pragma once

typedef struct files_memory_buffer
{
    char* data;
    long int len;
} files_memory_buffer;

extern struct files_memory_buffer files_read_file(const char* filename);
extern void files_free_memory_buffer(struct files_memory_buffer* buffer);