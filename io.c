#include "io.h"



void to_upper(char *s) {
    for (int i = 0; s[i] != '\0'; i++) {
        s[i] = (char)toupper((char)s[i]);
    }
}

char *readUntil(int fd, char separator) {
    char ch;
    int i = 0;
    int capacity = 16;

    char *buffer = malloc(capacity);
    if (buffer == NULL) {
        return NULL;
    }

    while (read(fd, &ch, 1) == 1) {
        if (ch == separator) {
            buffer[i] = '\0';
            return buffer;
        }

        if (i >= capacity - 1) {
            capacity *= 2;
            char *new_buf = realloc(buffer, capacity);
            if (new_buf == NULL) {
                free(buffer);
                return NULL;
            }
            buffer = new_buf;
        }

        buffer[i++] = ch;
    }

    /* EOF or error */
    if (i == 0) {
        free(buffer);
        return NULL;
    }

    buffer[i] = '\0';
    return buffer;
}

char *read_screen(){
    char c;
    int numOfChars = 0;
    int bytes;
    char *command;

    char *word = (char *)malloc(1);
    bytes = read(0, &c, 1);
    while (bytes > 0 && c != '\n') {
        char *tmp = (char *)realloc(word, (size_t)numOfChars + 2); // +1 for new char, +1 for '\0'

        word = tmp;

        word[numOfChars] = c;
        numOfChars++;

        bytes = read(0, &c, 1);
    }
    word[numOfChars] = '\0';
    command = word;
    return command;
}





