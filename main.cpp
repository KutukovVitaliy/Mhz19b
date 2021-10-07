#include "Mhz19B.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{
    bool rv;
    char commandRead[9] = {(char)0xff, (char)0x01, (char)0x86, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x79};
    Mhz19b::print_devices();
    std::string name = Mhz19b::get_port_name(0);
    std::cout << "Serial port: " << name << std::endl;

    Mhz19b serial;
    rv = serial.start(name.c_str(), 9600);
    if (rv == false) {
        return -1;
    }

    // initialize
    for(int i = 0; i < 3; ++i){
        serial.write_some(commandRead, 9);
        sleep(2);
        int res = serial.get_result();
        std::cout << res << std::endl;
    }

    serial.stop();

    return 0;
}
