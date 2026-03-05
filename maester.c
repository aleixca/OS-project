#include "maester.h"



void readMaester(char **argv){
    Maester maester;
    Routes *routes;
    char *buf;
    int i = 0;

    int fd = open(argv[0], O_RDONLY);
    if (fd == -1) {
        printF("Error opening file\n");
        return;
    }

    read_lines(fd, buf, 100);
    maester.realm_name = strdup(buf);
    read_lines(fd, buf, 100);
    maester.user_dir = strdup(buf);
    read_lines(fd, buf, 100);
    maester.envoy_count = atoi(buf);
    read_lines(fd, buf, 100);
    maester.listen_ip = strdup(buf);
    read_lines(fd, buf, 100);
    maester.listen_port = atoi(buf);
    read_lines(fd, buf, 100);
    read_lines(fd, buf, 100);
    
    while (read_lines(fd, buf, 100) > 0) {
        Route route;
        route.maester = strdup(buf);
        read_lines(fd, buf, 100);
        route.ip = strdup(buf);
        read_lines(fd, buf, 100);
        route.port = atoi(buf);

        Route *new_routes = realloc(routes, (maester.envoy_count + 1) * sizeof(Route));
        if (new_routes == NULL) {
            printF("Error reallocating memory for routes\n");
            free(routes);
            close(fd);
            return;
        }
        routes = new_routes;
        routes[i] = route;
        i++;
    }
    maester.route_count = i;
    maester.routes = routes;
    close(fd);

}



void list_realms(Maester maester) {
    int i = 0;
    
    for(i = 0; i < maester.route_count; i++) {
        char *output;
        int len = asprintf(&output, "\n - %s", maester.routes[i].maester);
        printF(output);
        free(output);
    }

}




