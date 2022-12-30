#include "functions-client.hpp"


void sendMessage(const int &sckt){
    char ch {};
    package_t package {};
    std::string message {};
    
    while ( (ch = (char) std::cin.get()) != STOP_INSERT_COMMAND ) message += ch;

    initPackage(package, TEXT_PACKAGE);
    strcpy(package.data, message.c_str());

    ssize_t ret {send(sckt, &package, sizeof(package_t), 0)}; 
    if ( ret >= 0 ){
        std::cerr << "Successful message sent" << std::endl;
        std::cerr << std::endl;
        return;
    }
    else if ( ret < 0 ){
        std::cerr << "Error sending message" << std::endl;
        std::cerr << std::endl;
        std::cerr << errno << std::endl;
        return;
    }
}


void sendFile(const int &sckt){
    std::string path;
    std::cin >> path;

    std::cerr << "Arquivo a ser enviado: " << path << std::endl;
    std::cerr << std::endl;
}


void quitProgram(bool &stop){
    stop = true;
    std::cerr << std::endl;
    std::cerr << "Bye! | Tchau! | Tchüss!" << std::endl;
    std::cerr << std::endl;
}
