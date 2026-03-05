#include "io.h"



void to_upper(char *s) {
    for (int i = 0; s[i] != '\0'; i++) {
        s[i] = (char)toupper((char)s[i]);
    }
}


int read_line(int fd, char *buf, int max) {
    if (!buf || max < 2) {
        return 0;
    }

    int i = 0;
    while (i < max - 1) {
        char c;
        int r = (int)read(fd, &c, 1);
        
        if (r <= 0) {
            break;       // EOF or error
        }

        buf[i++] = c;

        if (c == '\n'){
            break;              // Stop at newline
        }    
    }

    buf[i] = '\0';
    return i;
}





