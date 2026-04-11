#ifndef MAESTER_H
#define MAESTER_H

typedef struct  {
	char *maester;
	char *ip;
	int port;
} Route;

typedef struct {
char *realm_name; 
char *user_dir; 
int envoy_count; 
char *listen_ip; 
int listen_port;
char *origin; //IP and Port
Route *routes;
int route_count;
int stop_program;
int total_products;
int listen_fd;
} Maester;

#endif

