

#include "terminal.h"
#include "signal.h"
#include "io.h"
#include "maester.h"
#include "inventory.h"


#include <string.h>
#include <unistd.h>




int main(int argc, char *argv[]){
    
    signal_handler();
    read_Maester(argv[1]);
    load_inventory(argv[2]);
    terminal();



    return 0;
}
