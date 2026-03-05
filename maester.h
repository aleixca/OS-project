#ifndef MAESTER_H
#define MAESTER_H
#include "io.h"
#include "signal.h"
#include "terminal.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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
Route *routes;
int route_count;
} Maester;

void readMaester(char **argv);
void list_realms(int realm_count);

#endif
