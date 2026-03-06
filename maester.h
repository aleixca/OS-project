#ifndef MAESTER_H
#define MAESTER_H
#define _GNU_SOURCE


#include "io.h"


#include <fcntl.h>   
#include <unistd.h>
#include <stdio.h>
#include <string.h>
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
int stop_program;
int total_products;
} Maester;

Maester read_Maester(char *path);
void list_realms(Maester maester);
void free_Maester(Maester maester);
#endif

