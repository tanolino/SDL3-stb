#include "files.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static void
print_perror(const char* failing_message, const char* filename)
{
    printf("An error occured with \"%s\"\n", filename);
    perror(failing_message);
}

files_memory_buffer files_read_file(const char* filename)
{
    files_memory_buffer res = { 0 };
    if (!filename) {
        printf("Failed to open <NULL>-pointer filename");
        return res;
    }

    FILE* fptr = fopen(filename, "rb");
    if (!fptr) {
        print_perror("Failed to open file", filename);
        return res;
    }

    if (fseek(fptr, 0, SEEK_END)) {
        print_perror("Failed to SEEK_END", filename);
        goto CLOSE_AND_RETURN;
    }

    long int pos = ftell(fptr);
    rewind(fptr);
    if (pos == -1L) {
        print_perror("Failed to ftell(...)", filename);
        goto CLOSE_AND_RETURN;
    }

    char* buffer = malloc(pos);
    if (!buffer) {
        printf("Failed to read file: Out-of-Memory");
        goto CLOSE_AND_RETURN;
    }
    long int char_read = (long int)fread((void*)buffer, sizeof(char), (size_t)pos, fptr);
    if (char_read != pos)
    {
        free(buffer);
        if (ferror(fptr))
        {
            print_perror("Failed to fully read file", filename);
            goto CLOSE_AND_RETURN;
        }
    }

    res.data = buffer;
    res.len = pos;

CLOSE_AND_RETURN:
    if (fptr)
        fclose(fptr);
    return res;
}

void files_free_memory_buffer(files_memory_buffer* buffer)
{
    if (buffer)
    {
        if (buffer->data)
            free(buffer->data);
        buffer->data = NULL;
        buffer->len = 0;
    }
}
