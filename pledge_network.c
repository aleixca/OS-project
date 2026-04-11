#include "pledge_network.h"

void handle_pledge(char *realm, Maester maester) {
    // Find the route matching the realm name
    Route *route = NULL;
    for (int i = 0; i < maester.route_count; i++) {
        if (strcmp(maester.routes[i].maester, realm) == 0) {
            route = &maester.routes[i];
            break;
        }
    }

    if (route == NULL) {
        printF("Unknown realm.\n");
        return;
    }

    // Check not already pledged
    if (get_pledge_status(realm) != -1) {
        printF("Already pledged to this realm.\n");
        return;
    }

    int sock = connect_to_realm(route->ip, route->port);
    if (sock < 0) {
        printF("Could not connect to realm.\n");
        return;
    }

    Frame frame;
    build_frame(&frame, TYPE_ALLIANCE_REQUEST, maester.origin, realm, NULL, 0);

    if (!send_frame(sock, &frame)) {
        printF("Failed to send alliance request.\n");
        close_connection(sock);
        return;
    }

    // Wait for response
    Frame response;
    if (!receive_frame(sock, &response)) {
        printF("No response from realm.\n");
        close_connection(sock);
        return;
    }

    if (response.type == TYPE_ALLIANCE_RESPONSE) {
        add_pledge(realm);
        update_pledge_status(realm, 1); // ALLIED
        printF("Alliance established.\n");
    } else if (response.type == TYPE_NACK) {
        add_pledge(realm);
        update_pledge_status(realm, 2); // FAILED
        printF("Alliance rejected.\n");
    } else {
        printF("Unexpected response.\n");
    }

    close_connection(sock);
}

/********************
 *
 * @Name: handle_incoming_pledge
 * @Def: Handles an incoming alliance request from a remote realm.
 *       Stores the request as PENDING so the user can respond later.
 * @Arg: client_fd = connected socket of the incoming request
 *       frame = the received alliance request frame
 * @Ret: None
 *
 ********************/
void handle_incoming_pledge(int client_fd, Frame *frame) {
    char *output;
    int len;
    char realm[21];
 
    strncpy(realm, frame->origin, 20);
    realm[20] = '\0';
 
    if (get_pledge_status(realm) != -1) {
        Frame response;
        build_frame(&response, TYPE_NACK, realm, frame->origin, NULL, 0);
        send_frame(client_fd, &response);
        close_connection(client_fd);
        return;
    }
 
    add_pledge(realm);
 
    len = asprintf(&output, "\nIncoming alliance request from %s. Use PLEDGE RESPOND %s ACCEPT/REJECT.\n", realm, realm);
    if (len != -1) {
        printF(output);
        free(output);
    }
 
    close_connection(client_fd);
}

/********************
 *
 * @Name: handle_pledge_respond
 * @Def: Responds to a pending incoming alliance request.
 * @Arg: realm = name of the realm to respond to
 *       maester = the Maester instance
 *       accept = 1 to accept, 0 to reject
 * @Ret: None
 *
 ********************/
void handle_pledge_respond(char *realm, Maester maester, int accept) {
    char *output;
    int len;
 
    if (get_pledge_status(realm) != 0) {
        printF("No pending alliance request from that realm.\n");
        return;
    }
 
    Route *route = NULL;
    for (int i = 0; i < maester.route_count; i++) {
        if (strcmp(maester.routes[i].maester, realm) == 0) {
            route = &maester.routes[i];
            break;
        }
    }
 
    if (route == NULL) {
        printF("Unknown realm.\n");
        return;
    }
 
    int sock = connect_to_realm(route->ip, route->port);
    if (sock < 0) {
        printF("Could not connect to realm.\n");
        return;
    }
 
    Frame frame;
    unsigned char type = accept ? TYPE_ALLIANCE_RESPONSE : TYPE_NACK;
    build_frame(&frame, type, maester.origin, realm, NULL, 0);
 
    if (!send_frame(sock, &frame)) {
        printF("Failed to send response.\n");
        close_connection(sock);
        return;
    }
 
    update_pledge_status(realm, accept ? 1 : 2);
 
    len = asprintf(&output, "Alliance %s.\n", accept ? "accepted" : "rejected");
    if (len != -1) {
        printF(output);
        free(output);
    }
 
    close_connection(sock);
}