#ifndef PLEDGE_NETWORK_H
#define PLEDGE_NETWORK_H

#include "types.h"
#include "network.h"
#include "pledge.h"
#include "io.h"

void handle_pledge(char *realm, Maester maester);
void handle_incoming_pledge(int client_fd, Frame *frame);
void handle_pledge_respond(char *realm, Maester maester, int accept);

#endif