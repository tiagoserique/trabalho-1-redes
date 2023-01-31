#ifndef FUNCOES_CLIENT_H 
#define FUNCOES_CLIENT_H


// includes ====================================================================

#include <string>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "utils.hpp"
#include "ConexaoRawSocket.h"


// header functions ============================================================

/*
    @brief Send the message using the socket

    @param sckt (const int &) : The File Descriptor of the socket

    @return void
*/
void sendMessage(const int &sckt);

/*
    @brief Send the file using the socket

    @param sckt (const int &) : The File Descriptor of the socket

    @return void
*/
void sendFile(const int &sckt);

/*
    @brief Sends an array of packs using sliding windows of size 5

    @param sckt (const int &) : The File Descriptor of the socket
    @param packs (package_t *) : The array of packages to be sent, the first one should not be of type START_PACKAGE, the last one being of type END_PACKAGE

    @return the number of packages sent
    @return 0 if no packages could be sent
*/
unsigned int sendPacks(const int &sckt, package_t * packs);

/*
    @brief Quit the program

    @param stop (bool &) : The variable that controls the program

    @return void
*/
void quitProgram(bool &stop);


#endif
