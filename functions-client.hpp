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
    @brief Quit the program

    @param stop (bool &) : The variable that controls the program

    @return void
*/
void quitProgram(bool &stop);


#endif