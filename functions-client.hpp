#ifndef FUNCOES_CLIENT_H 
#define FUNCOES_CLIENT_H


// --------------------------
// Includes
// --------------------------

#include <string>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "utils.hpp"
#include "ConexaoRawSocket.h"


// --------------------------
// Header Functions
// --------------------------

/*
    @brief Send the message using the socket

    @param sckt (const int &) : The File Descriptor of the socket
    @param seq  (cont int)    : Starting sequence number of to be used on the 
    messages being sent
    @param username (const std::string &) : The username of the client

    @return Sequence number of the last package sent
    @return -1 in error
*/
int sendMessage(const int &sckt, const int &seq, const std::string &username);

/*
    @brief Send the file using the socket

    @param sckt (const int &) : The File Descriptor of the socket
    @param seq  (cont int &)  : Starting sequence number of to be used on the 
    messages being sent
    @param username (const std::string &) : The username of the client

    @return Sequence number of the last package sent
    @return -1 in error
*/
int sendFile(const int &sckt, const int &seq, const std::string &username);

/*
    @brief Quit the program

    @param stop (bool &) : The variable that controls the program

    @return void
*/
void quitProgram(bool &stop);


// --------------------------
// Internal Header Functions
// --------------------------

/*
    @brief Waits for Ack/Nack

    @param sckt (const int &) : The File Descriptor of the socket
    @param type (const uint32_t) : The type of package to wait

    @return -1 if timed out
    @return The sequence number of the ack package otherwise
*/
int waitResponse(const int &sckt, const uint32_t type);

/*
    @brief Sends an array of packs using sliding windows of size 5

    @param sckt (const int &) : The File Descriptor of the socket
    @param packs (package_t *) : The array of packages to be sent, the first one
     should not be of type START_PACKAGE, the last one being of type END_PACKAGE

    @return the number of packages sent
    @return 0 if no packages could be sent
*/
uint32_t sendPacks(const int &sckt, package_t * packs);


#endif
