#include "io.h"


/********************
 *
 * @Name: to_upper
 * @Def: Converts a string to uppercase.
 * @Arg: s = pointer to the string to convert
 * @Ret: None
 *
 ********************/
void to_upper(char *s) {
    for (int i = 0; s[i] != '\0'; i++) {
        s[i] = (char)toupper((char)s[i]);
    }
}

/********************
 *
 * @Name: add_newline
 * @Def: Adds a newline character to the end of a string.
 * @Arg: str = pointer to the string to modify
 * @Ret: Pointer to the modified string, or NULL when it fails
 *
 ********************/
char *add_newline(char *str) {
    int len = strlen(str);

    char *tmp = realloc(str, len + 2);   
    if (tmp == NULL) {
        free(str);
        return NULL;
    }

    str = tmp;
    str[len] = '\n';
    str[len + 1] = '\0';
    return str;
} 

/********************
 *
 * @Name: remove_slashr
 * @Def: Removes the carriage return character from a string.
 * @Arg: s = pointer to the string to modify
 * @Ret: None
 *
 ********************/
void remove_slashr(char *s) {
    int i = 0;
    while (s[i] != '\0') {
        if (s[i] == '\r') {
            s[i] = '\0';
            return;
        }
        i++;
    }
}

/********************
 *
 * @Name: readUntil
 * @Def: Reads characters from a file descriptor until a specified separator is encountered.
 * @Arg: fd = file descriptor to read from
 *       separator = character that indicates the end of reading
 * @Ret: Pointer to the read string, or NULL when it fails
 *
 ********************/
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

/********************
 *
 * @Name: read_screen
 * @Def: Reads a line of input from the user.
 * @Arg: None
 * @Ret: Pointer to the read string, or NULL when it fails.
 *
 ********************/
char *read_screen() {
    char c;
    int numOfChars = 0;
    int bytes;
    char *word = malloc(1);

    if (word == NULL) {
        return NULL;
    }

    while (1) {
        bytes = read(0, &c, 1);

        if (bytes <= 0) {
            free(word);
            return NULL;
        }

        if (c == '\n') {
            break;
        }

        char *tmp = realloc(word, numOfChars + 2);
        if (tmp == NULL) {
            free(word);
            return NULL;
        }

        word = tmp;
        word[numOfChars] = c;
        numOfChars++;
    }

    word[numOfChars] = '\0';
    return word;
}
