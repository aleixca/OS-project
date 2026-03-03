#include "io.h"
#include <unistd.h>
#include <string.h>
#include <ctype.h>


void to_upper_inplace(char *s) {
    if (!s) return;
    for (size_t i = 0; s[i] != '\0'; i++) {
        s[i] = (char)toupper((unsigned char)s[i]);
    }
}


