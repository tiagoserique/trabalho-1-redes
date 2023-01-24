#include "functions-client.hpp"


void sendMessage(const int &sckt){
    char ch {};
    package_t start {};
    std::string message {};
    
    while ( (ch = (char) std::cin.get()) != STOP_INSERT_COMMAND ) message += ch;

    initPackage(start, START_PACKAGE);
    ssize_t ret {send(sckt, &start, sizeof(package_t), 0)}; 
    if ( ret < 0 ){
        std::cerr << "Error sending message" << std::endl;
        std::cerr << std::endl;
        std::cerr << errno << std::endl;
        return;
    }
    package_t * packs {divideData(
                      (void *)message.c_str(),
                      message.size(),
                      TEXT_PACKAGE,
                      0,
                      0 /* Client */)};

    unsigned int packI {0};
    package_t currPack = packs[0];
    while ( currPack.type != END_PACKAGE ){
        currPack = packs[packI];
        ret = send(sckt, &currPack, sizeof(package_t), 0); 
        if ( ret < 0 ){
            std::cerr << "Error sending message" << std::endl;
            std::cerr << std::endl;
            std::cerr << errno << std::endl;
            return;
        }
        packI++;
    }
    std::cerr << "Successful message sent" << std::endl;
    std::cerr << std::endl;
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
