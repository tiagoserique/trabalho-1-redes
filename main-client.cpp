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

    system("clear");
    std::cout << "=============================" << std::endl;
    std::cout << "\t Client \t" << std::endl;
    std::cout << "=============================" << std::endl;


    std::cout << "Enter your username: ";
    std::string username {};
    std::cin >> username;

    system("clear");
    while ( !stop ){
        std::cout << "=============================" << std::endl;
        std::cout << "\t Commands \t" << std::endl;
        std::cout << "=============================" << std::endl;
        std::cout << "Insert message: " << INSERT_COMMAND << std::endl;
        std::cout << "Send file: " << SEND_COMMAND << std::endl;
        std::cout << "Quit: " << QUIT_COMMAND << std::endl;
        std::cout << "=============================" << std::endl;

        std::cout << "Enter command: ";
        std::string command {};
        std::cin >> command;
        std::cin.ignore();

        if ( INSERT_COMMAND == command ){
            seq = sendMessage(socket, seq, username) + 1;
        }
        if ( SEND_COMMAND == command ){
            seq = sendFile(socket, seq, username) + 1;
        }
        if ( QUIT_COMMAND == command ){
            quitProgram(stop);
        }
        std::cerr << "New sequence number is: " << seq << std::endl;
    }
}
