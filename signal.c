#include "signal.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>

int stop_program = 0;

void signal_handler(){

  write(1, "Exiting program gracefully...\n", strlen("Exiting program gracefully...\n"));
   stop_program = 1

}


int main (){


  signal(SIGINT, signal_handler);

return 0;
}
