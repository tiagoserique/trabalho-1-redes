
#include "functions-server.hpp"


void receivePackage(const int &sckt, package_t &pckg){
    ssize_t val {recv(sckt, &pckg, sizeof(pckg), 0)};

    if ( val >= 0 ){
        checkTypePackage(sckt, pckg);
    }
    else {
        std::cout << "Error receiving message" << std::endl;
    }
}


void checkTypePackage(const int &sckt, package_t &pckg){
    // TODO: Check crc if error send nack and return
    switch ( pckg.type ){
        case TEXT_PACKAGE:
            handleMessages(pckg);
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
    package_t ack {};
    initPackage( ack, ACK_PACKAGE );
    ack.sequence = pckg.sequence;
    ssize_t ret {send(sckt, &ack, sizeof(package_t), 0)}; 
    if ( ret < 0 ){
        std::cerr << "Error sending ack" << std::endl;
        std::cerr << std::endl;
        std::cerr << errno << std::endl;
        return;
    }
}


void handleMessages(const package_t &pckg){
    if ( !validateCRC(pckg) ){
        return;
    }

    // message's format
    // [<date e hour>]<User> : message
    
    printDate();
    std::cout << "<Usuário> : " << pckg.data << std::endl << std::endl;
}


void handleMedias(){
    // File message's format
    // [<data e hour>]<User> : Arquivo de midia enviado! O arquivo pode ser encontrado em <caminho_para_arquivo>

    printDate();
    std::cout << "<Usuário> : " 
        << "Arquivo de midia enviado! O arquivo pode ser encontrado em " 
        << "<caminho_para_arquivo>" 
        << std::endl << std::endl;
}
