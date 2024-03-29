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
    @brief Handles printings messages

    @param data (package_t *) : The packages received

    @return void
*/
void handleMessages(package_t *packs);

/*
    @brief Receive the package using the socket

    @param sckt (const int &) : The File Descriptor of the socket
    @param pckg (package_t &) : The reference of package

    @return void
*/
void receivePackage(const int &sckt, package_t &pckg);

/*
    @brief Receive an array of packages using the socket

    @param sckt (const int &) : The File Descriptor of the socket
    @param seq (uint32_t)     : The sequence number received packages should start

    @return The array of packages received the last one should be of type END_PACKAGE
*/
package_t * receiveOtherPacks(const int &sckt, uint32_t seq);

/*
    @brief Merges the data in a sequence of packages and displays what is needed

    @param packs (const int &) : The File Descriptor of the socket
    @param packs (package_t *) : The array of packages

    @return void    
*/
void handlePacks(const int &sckt, package_t * packs);

/*
    @brief Handle the case of receive medias

    @return void
*/
void handleMedias(package_t *packs);
 

#endif
