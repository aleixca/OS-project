#include "signal.h"


int stop_program = 0;

void signal_handler(int sig){
  (void)sig;
  printF("Exiting program gracefully...\n");
   stop_program = 1;

}

void setup_signal(void) {
    signal(SIGINT, signal_handler);
}


