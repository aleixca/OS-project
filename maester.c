#include "maester.h"


Maester read_Maester(char *path){
    Maester maester;
    char *buf;
    int i = 0;
    char *output;
    maester.routes = NULL;
    maester.route_count = 0;

    int fd = open(path, O_RDONLY);
    
    if (fd == -1) {
        printF("Error opening file\n");
        Maester empty_maester = {0};
        return empty_maester;
    }


  
    buf = readUntil(fd, '\n');
    remove_slashr(buf);
    maester.realm_name = strdup(buf);
    free(buf);
    
    buf = readUntil(fd, '\n');
    remove_slashr(buf);
    maester.user_dir = strdup(buf);
    free(buf);
    
    buf = readUntil(fd, '\n');
    remove_slashr(buf);
    maester.envoy_count = atoi(buf);
    free(buf);
    
    buf = readUntil(fd, '\n');
    remove_slashr(buf);
    maester.listen_ip = strdup(buf);
    free(buf);
    
    buf = readUntil(fd, '\n');
    remove_slashr(buf);
    maester.listen_port = atoi(buf);
    free(buf);
    
    buf = readUntil(fd, '\n'); //read trash line (---ROUTES---)
    remove_slashr(buf);
    free(buf);

    while ((buf = readUntil(fd, ' ')) != NULL) {
        Route temp;
        for (int j = 0; buf[j] != '\0'; j++) {
            if (buf[j] == '&') {
                buf[j] = ' ';
                break;
            }
        }
        temp.maester = strdup(buf);
        free(buf);
        buf = readUntil(fd, ' ');
        temp.ip = strdup(buf);
        free(buf);
        buf = readUntil(fd, '\n');
        temp.port = atoi(buf);
        free(buf);

        Route *new_routes = realloc(maester.routes, (i + 1) * sizeof(Route));
        if (new_routes == NULL) {
            printF("Error reallocating memory for routes\n");
            free(maester.routes);
            close(fd);
            return maester;
        }
        maester.routes = new_routes;
        maester.routes[i] = temp;
        i++;
    }
    maester.route_count = i;
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
        int len = asprintf(&output, " - %s\n", maester.routes[i].maester);
        if(len == -1) {
            printF("Error creating output string\n");
            return;
        }
        printF(output);
        free(output);
    }

}

void exit_maester(Maester maester) {
    char *output;

    int len = asprintf(&output, "\nMaester of %s signing off. The board is cleared.\n", maester.realm_name);
    if (len != -1) {
        printF(output);
        free(output);
    }

}

void free_Maester(Maester maester) {
    free(maester.realm_name);
    free(maester.user_dir);
    free(maester.listen_ip);
    for (int i = 0; i < maester.route_count; i++) {
        free(maester.routes[i].maester);
        free(maester.routes[i].ip);
    }
    free(maester.routes);
}







