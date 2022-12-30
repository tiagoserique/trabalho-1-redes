#include "functions-server.hpp"


int main(){
    bool stop {false};

    int socket {cria_raw_socket((char *) "lo")};
    if ( socket < 0 ){
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }


    std::cout << "=============================" << std::endl;
    std::cout << "\t Server \t" << std::endl;
    std::cout << "=============================" << std::endl;


    while( !stop ){
        package_t package;

        receivePackage(socket, package);
    }
}
