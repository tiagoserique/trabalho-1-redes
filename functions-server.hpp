#ifndef FUNCOES_SERVER_H 
#define FUNCOES_SERVER_H


// includes ====================================================================

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "utils.hpp"
#include "ConexaoRawSocket.h"


// header functions ============================================================

/*
    @brief Handle the case of receive messages

    @param msg (const int &) : The message received

    @return void
*/
void handleMessages(const package_t &pckg);

/*
    @brief Receive the package using the socket

    @param sckt (const int &) : The File Descriptor of the socket
    @param pckg (package_t &) : The reference of package

    @return void
*/
void receivePackage(const int &sckt, package_t &pckg);

/*
    @brief Check the type of package

    @param pckg (package_t &) : The reference of package
    @param sckt (const int &) : Socket to send ack/nack

    @return void    
*/
void checkTypePackage(const int &sckt, package_t &pckg);

/*
    @brief Handle the case of receive medias

    @return void
*/
void handleMedias();
 

#endif
