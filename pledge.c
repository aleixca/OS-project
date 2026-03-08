#include "pledge.h"

static Pledge pledges[MAX_PLEDGES];
static int pledge_count = 0;

/********************
 *
 * @Name: init_pledges
 * @Def: Initializes the pledge system.
 * @Arg: None
 * @Ret: None
 *
 ********************/
void init_pledges(void) {
    pledge_count = 0;
    for (int i = 0; i < MAX_PLEDGES; i++) {
        pledges[i].realm_name[0] = '\0';
        pledges[i].status = 0;
    }
}

/********************
 *
 * @Name: add_pledge
 * @Def: Adds a new pledge to the system.
 * @Arg: realm = name of the realm to pledge
 * @Ret: 0 on success, -1 on failure
 *
 ********************/
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

/********************
 *
 * @Name: show_pledge_status
 * @Def: Displays the status of all pledges.
 * @Arg: None
 * @Ret: None
 *
 ********************/
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

/********************
 *
 * @Name: get_pledge_status
 * @Def: Retrieves the status of a specific pledge.
 * @Arg: realm = name of the realm to check
 * @Ret: Status of the pledge (0: PENDING, 1: ALLIED, 2: FAILED), or -1 if not found
 *
 ********************/
int get_pledge_status(char *realm) {
    for (int i = 0; i < pledge_count; i++) {
        if (strcmp(pledges[i].realm_name, realm) == 0) {
            return pledges[i].status;
        }
    }
    return -1;
}

/********************
 *
 * @Name: update_pledge_status
 * @Def: Updates the status of a specific pledge.
 * @Arg: realm = name of the realm to update
 *     status = new status of the pledge (0: PENDING, 1: ALLIED, 2: FAILED)
 * @Ret: None
 *
 ********************/
void update_pledge_status(char *realm, int status) {
    for (int i = 0; i < pledge_count; i++) {
        if (strcmp(pledges[i].realm_name, realm) == 0) {
            pledges[i].status = status;
            return;
        }
    }
}

/********************
 *
 * @Name: get_pledge_realm
 * @Def: Retrieves the realm name of a specific pledge.
 * @Arg: index = index of the pledge to retrieve
 * @Ret: Realm name of the pledge, or NULL if not found
 *
 ********************/
char *get_pledge_realm(int index) {
    if (index >= 0 && index < pledge_count) {
        return pledges[index].realm_name;
    }
    return NULL;
}

/********************
 *
 * @Name: get_pledge_count
 * @Def: Retrieves the total number of active pledges.
 * @Arg: None
 * @Ret: Total number of active pledges
 *
 ********************/
int get_pledge_count(void) {
    return pledge_count;
}