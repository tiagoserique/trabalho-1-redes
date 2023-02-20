#include "functions-client.hpp"


int main(){
    bool stop {false};
    int seq = 0;

    int socket {cria_raw_socket((char *) "lo")};
    if ( socket < 0 ){
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }
    setSockTimeout(socket, 100000);


    std::cout << "=============================" << std::endl;
    std::cout << "\t Client \t" << std::endl;
    std::cout << "=============================" << std::endl;


    while ( !stop ){
        std::string command {};
        std::cin >> command;
        std::cin.ignore();

        if ( INSERT_COMMAND == command ){
            seq = sendMessage(socket, seq) + 1;
        }
        if ( SEND_COMMAND == command ){
            sendFile(socket);
        }
        if ( QUIT_COMMAND == command ){
            quitProgram(stop);
        }
        std::cerr << "New sequence number is: " << seq << std::endl;
    }
}
