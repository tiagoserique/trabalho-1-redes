
#include "functions-server.hpp"


void receivePackage(const int &sckt, package_t &pckg){
    ssize_t val {recv(sckt, &pckg, sizeof(pckg), 0)};

    if ( val >= 0 ){
        checkTypePackage(pckg);
    }
    else {
        std::cout << "Error receiving message" << std::endl;
    }
}


void checkTypePackage(package_t &pckg){
    switch ( pckg.type ){
        case TEXT_PACKAGE:
            handleMessages(pckg.data);
            break;

        case MEDIA_PACKAGE:
            handleMedias();
            break;

        // case ACK_PACKAGE:
        //     break;

        // case NACK_PACKAGE:
        //     break;

        // case ERROR_PACKAGE:
        //     break;

        // case START_PACKAGE:
        //     break;

        // case END_PACKAGE:
        //     break;

        // case DATA_PACKAGE:
        //     break;

        default:
            break;
    }
}


void handleMessages(const char *msg){
    // format
    // [<date e hour>]<User> : message
    
    printDate();
    std::cout << "<Usuário> : " << msg << std::endl << std::endl;
}


void handleMedias(){
    // format
    // [<data e hora>]<Usuário> : Arquivo de midia enviado! O arquivo pode ser encontrado em <caminho_para_arquivo>

    printDate();
    std::cout << "<Usuário> : " 
        << "Arquivo de midia enviado! O arquivo pode ser encontrado em " 
        << "<caminho_para_arquivo>" 
        << std::endl << std::endl;
}
