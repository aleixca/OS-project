#ifndef IO_H
#define IO_H

#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <sys/types.h>



#define printF(x) write(STDOUT_FILENO, x, strlen(x))

void to_upper(char *str);

int read_line(int fd, char *buf, int max);

#endif
