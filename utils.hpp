#ifndef UTILS_HPP
#define UTILS_HPP


// includes ====================================================================

#include <cstdlib>
#include <stdlib.h>
#include <net/if.h>
#include <iostream>
#include <array>


// typedefs ====================================================================

typedef struct package_t {
    unsigned int header   : 8; 
    unsigned int type     : 6; 
    unsigned int sequence : 4; 
    unsigned int size     : 6; 
    char data[64]; 
    unsigned int crc      : 8;
} package_t;


// constraints =================================================================

// mark of package
constexpr static unsigned int PACKAGE_START_MARK {0x7E}; // marca: 0x7E

// types of package
constexpr static unsigned int TEXT_PACKAGE  {0x01}; // texto: 0x01
constexpr static unsigned int MEDIA_PACKAGE {0x10}; // mídia: 0x10
constexpr static unsigned int ACK_PACKAGE   {0x0A}; // ack: 0x0A
constexpr static unsigned int NACK_PACKAGE  {0x00}; // nack: 0x00
constexpr static unsigned int ERROR_PACKAGE {0x1E}; // erro: 0x1E
constexpr static unsigned int START_PACKAGE {0x1D}; // inicio de transmissão: 0x1D
constexpr static unsigned int END_PACKAGE   {0x0F}; // fim de transmissão: 0x0F
constexpr static unsigned int DATA_PACKAGE  {0x0D}; // dados: 0x0D

// address of server and client
constexpr static unsigned int SERVER_ADDRESS {0x01};
constexpr static unsigned int CLIENT_ADDRESS {0x02};

// commands
const short int   STOP_INSERT_COMMAND {27};
const std::string INSERT_COMMAND      {"i"};
const std::string QUIT_COMMAND        {":q"};
const std::string SEND_COMMAND        {":send"};


// header functions ============================================================

/*
    @brief Init the package with the type of package

    @param pckg (package_t &) : The reference of package
    @param type (unsigned int) : The type of package

    @return void
*/
void initPackage(package_t &pckg, unsigned int type);

/*
    @brief Divides the data sent into an array of packages

    @param data (void *)       : An array with the data that is going to be sent
    @param size (unsigned int) : The size of the array data
    @param type (unsigned int) : The type of the packages to be created
    @param seq  (unsigned int) : The desired sequence number of the starting package

    @return Returns an array of packages containing the data sent, the last element will have type END_PACKAGE
    @return Returns NULL if there is an error
*/
package_t * divideData(void * data, unsigned int size, unsigned int type, unsigned int seq);

/*
    @brief Print the package

    @param pckg (const package_t &) : The reference of package

    @return void
*/
void printPackage(const package_t &pckg);

/*
    @brief Print the date and hour

    @return void
*/
void printDate();

/*
    @brief Sets the time to wait for receiving/sending on a socket

    @param sckt (cont int &) : socket to set the timeout timer
    @param useconds (unsigned int) : time in microseconds to set the timeout (1.000.000=1 second)

    @return void
*/
void setSockTimeout(const int &sckt, const int useconds);


#endif
