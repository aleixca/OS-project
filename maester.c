#include "maester.h"


Maester read_Maester(char *path){
    Maester maester;
    Route *routes = NULL;
    char *buf;
    int i = 0;
    char *output;

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        printF("Error opening file\n");
        Maester empty_maester = {0};
        return empty_maester;
    }


  
    buf = readUntil(fd, '\n');
    printF(buf);
    maester.realm_name = buf;
    buf = readUntil(fd, '\n');
    maester.user_dir = strdup(buf);
    buf = readUntil(fd, '\n');
    maester.envoy_count = atoi(buf);
    buf = readUntil(fd, '\n');
    maester.listen_ip = strdup(buf);
    buf = readUntil(fd, '\n');
    maester.listen_port = atoi(buf);
    buf = readUntil(fd, '\n');

    while (readUntil(fd, '\n') != NULL) {
        Route temp;
        temp.maester = strdup(buf);
        buf = readUntil(fd, '\n');
        temp.ip = strdup(buf);
        buf = readUntil(fd, '\n');
        temp.port = atoi(buf);

        Route *new_routes = realloc(routes, (maester.envoy_count + 1) * sizeof(Route));
        if (new_routes == NULL) {
            printF("Error reallocating memory for routes\n");
            free(routes);
            close(fd);
            return maester;
        }
        routes = new_routes;
        routes[i] = temp;
        i++;
    }
    maester.route_count = i;
    maester.routes = routes;
    close(fd);

    int len = asprintf(&output, "Maester of %s initialized. The board is set.\n", maester.realm_name);
    if (len != -1) {
        printF(output);
        free(output);
    }

    return maester;
}



void list_realms(Maester maester) {
    int i = 0;
    
    for(i = 0; i < maester.route_count; i++) {
        char *output;
        int len = asprintf(&output, "\n - %s", maester.routes[i].maester);
        if(len == -1) {
            printF("Error creating output string\n");
            return;
        }
        printF(output);
        free(output);
    }

}


void free_Maester(Maester maester) {
    free(maester.realm_name);
    free(maester.user_dir);
    for (int i = 0; i < maester.route_count; i++) {
        free(maester.routes[i].maester);
        free(maester.routes[i].ip);
    }
    free(maester.routes);
}





