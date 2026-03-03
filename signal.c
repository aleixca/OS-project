#include "signal.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>

int stop_program = 0;

void signal_handler(int sig){
  (void)sig;
  write(1, "Exiting program gracefully...\n", strlen("Exiting program gracefully...\n"));
   stop_program = 1;

}

void setup_signal(void) {
    signal(SIGINT, signal_handler);
}



