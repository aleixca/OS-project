#include "pledge.h"

static Pledge pledges[MAX_PLEDGES];
static int pledge_count = 0;

void init_pledges(void) {
    pledge_count = 0;
    for (int i = 0; i < MAX_PLEDGES; i++) {
        pledges[i].realm_name[0] = '\0';
        pledges[i].status = 0;
    }
}

int add_pledge(char *realm) {
    if (pledge_count >= MAX_PLEDGES) {
        return -1;
    }
    
    // Check if already exists
    for (int i = 0; i < pledge_count; i++) {
        if (strcmp(pledges[i].realm_name, realm) == 0) {
            return -1;
        }
    }
    
    strcpy(pledges[pledge_count].realm_name, realm);
    pledges[pledge_count].status = 0;
    pledge_count++;
    
    return pledge_count;
}

void show_pledge_status(void) {
    char *output;
    int len;
    
    len = asprintf(&output, "$ PLEDGE STATUS\n");
    if (len != -1) {
        printF(output);
        free(output);
    }
    
    for (int i = 0; i < pledge_count; i++) {
        char *status_str;
        if (pledges[i].status == 0) status_str = "PENDING";
        else if (pledges[i].status == 1) status_str = "ALLIED";
        else status_str = "FAILED";
        
        len = asprintf(&output, "- %s: %s\n", pledges[i].realm_name, status_str);
        if (len != -1) {
            printF(output);
            free(output);
        }
    }
}

int get_pledge_status(char *realm) {
    for (int i = 0; i < pledge_count; i++) {
        if (strcmp(pledges[i].realm_name, realm) == 0) {
            return pledges[i].status;
        }
    }
    return -1;
}

void update_pledge_status(char *realm, int status) {
    for (int i = 0; i < pledge_count; i++) {
        if (strcmp(pledges[i].realm_name, realm) == 0) {
            pledges[i].status = status;
            return;
        }
    }
}

char *get_pledge_realm(int index) {
    if (index >= 0 && index < pledge_count) {
        return pledges[index].realm_name;
    }
    return NULL;
}

int get_pledge_count(void) {
    return pledge_count;
}