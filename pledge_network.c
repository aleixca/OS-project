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