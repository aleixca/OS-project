#ifndef PLEDGE_H
#define PLEDGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "io.h"

#define MAX_PLEDGES 100

typedef struct {
    char realm_name[50];
    int status; // 0 = pending, 1 = allied, 2 = failed
} Pledge;

void init_pledges(void);
int add_pledge(char *realm);
void show_pledge_status(void);
int get_pledge_status(char *realm);
void update_pledge_status(char *realm, int status);
char *get_pledge_realm(int index);
int get_pledge_count(void);

#endif