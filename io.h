#ifndef IO_H
#define IO_H

#include "signal.h"
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdlib.h>



#define printF(x) write(STDOUT_FILENO, x, strlen(x))

void to_upper(char *str);
char *add_newline(char *s);
void remove_slashr(char *str);
char *readUntil(int fd, char separator);

char *read_screen();

#endif
