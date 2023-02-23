#ifndef UTILS_HPP
#define UTILS_HPP


// --------
// Includes
// --------

#include <cstdlib>
#include <stdlib.h>
#include <net/if.h>
#include <iostream>
#include <array>
#include <fstream>
#include <filesystem>


// --------
// Tipedefs
// --------

typedef struct package_t {
    uint32_t header       : 8; // header of package
    uint32_t type         : 6; // type of package
    uint32_t seq          : 4; // sequence number of package (0-15)
    uint32_t size         : 6; // size of data
    unsigned char data[63];    // data
    unsigned char crc     : 8; // crc8 wcdma generate with 0x9B
} package_t;


// --------
// Constraints
// --------

// mark of package
constexpr static uint32_t PACKAGE_START_MARK {0x7E}; // marca: 0x7E

// types of package
constexpr static uint32_t TEXT_PACKAGE  {0x01}; // texto: 0x01
constexpr static uint32_t MEDIA_PACKAGE {0x10}; // mídia: 0x10
constexpr static uint32_t ACK_PACKAGE   {0x0A}; // ack: 0x0A
constexpr static uint32_t NACK_PACKAGE  {0x00}; // nack: 0x00
constexpr static uint32_t ERROR_PACKAGE {0x1E}; // erro: 0x1E
constexpr static uint32_t START_PACKAGE {0x1D}; // inicio de transmissão: 0x1D
constexpr static uint32_t END_PACKAGE   {0x0F}; // fim de transmissão: 0x0F
constexpr static uint32_t DATA_PACKAGE  {0x0D}; // dados: 0x0D

// address of server and client
constexpr static uint32_t SERVER_ADDRESS {0x01};
constexpr static uint32_t CLIENT_ADDRESS {0x02};

// commands
constexpr static uint8_t STOP_INSERT_COMMAND {27};
const static std::string INSERT_COMMAND      {"i"};
const static std::string QUIT_COMMAND        {":q"};
const static std::string SEND_COMMAND        {":send"};

// crc8 table
constexpr static unsigned char crc8x_table[] = {
    0x00, 0x9b, 0xad, 0x36, 0xc1, 0x5a, 0x6c, 0xf7, 0x19, 0x82, 0xb4, 0x2f, 0xd8,
    0x43, 0x75, 0xee, 0x32, 0xa9, 0x9f, 0x04, 0xf3, 0x68, 0x5e, 0xc5, 0x2b, 0xb0,
    0x86, 0x1d, 0xea, 0x71, 0x47, 0xdc, 0x64, 0xff, 0xc9, 0x52, 0xa5, 0x3e, 0x08,
    0x93, 0x7d, 0xe6, 0xd0, 0x4b, 0xbc, 0x27, 0x11, 0x8a, 0x56, 0xcd, 0xfb, 0x60,
    0x97, 0x0c, 0x3a, 0xa1, 0x4f, 0xd4, 0xe2, 0x79, 0x8e, 0x15, 0x23, 0xb8, 0xc8,
    0x53, 0x65, 0xfe, 0x09, 0x92, 0xa4, 0x3f, 0xd1, 0x4a, 0x7c, 0xe7, 0x10, 0x8b,
    0xbd, 0x26, 0xfa, 0x61, 0x57, 0xcc, 0x3b, 0xa0, 0x96, 0x0d, 0xe3, 0x78, 0x4e,
    0xd5, 0x22, 0xb9, 0x8f, 0x14, 0xac, 0x37, 0x01, 0x9a, 0x6d, 0xf6, 0xc0, 0x5b,
    0xb5, 0x2e, 0x18, 0x83, 0x74, 0xef, 0xd9, 0x42, 0x9e, 0x05, 0x33, 0xa8, 0x5f,
    0xc4, 0xf2, 0x69, 0x87, 0x1c, 0x2a, 0xb1, 0x46, 0xdd, 0xeb, 0x70, 0x0b, 0x90,
    0xa6, 0x3d, 0xca, 0x51, 0x67, 0xfc, 0x12, 0x89, 0xbf, 0x24, 0xd3, 0x48, 0x7e,
    0xe5, 0x39, 0xa2, 0x94, 0x0f, 0xf8, 0x63, 0x55, 0xce, 0x20, 0xbb, 0x8d, 0x16,
    0xe1, 0x7a, 0x4c, 0xd7, 0x6f, 0xf4, 0xc2, 0x59, 0xae, 0x35, 0x03, 0x98, 0x76,
    0xed, 0xdb, 0x40, 0xb7, 0x2c, 0x1a, 0x81, 0x5d, 0xc6, 0xf0, 0x6b, 0x9c, 0x07,
    0x31, 0xaa, 0x44, 0xdf, 0xe9, 0x72, 0x85, 0x1e, 0x28, 0xb3, 0xc3, 0x58, 0x6e,
    0xf5, 0x02, 0x99, 0xaf, 0x34, 0xda, 0x41, 0x77, 0xec, 0x1b, 0x80, 0xb6, 0x2d,
    0xf1, 0x6a, 0x5c, 0xc7, 0x30, 0xab, 0x9d, 0x06, 0xe8, 0x73, 0x45, 0xde, 0x29,
    0xb2, 0x84, 0x1f, 0xa7, 0x3c, 0x0a, 0x91, 0x66, 0xfd, 0xcb, 0x50, 0xbe, 0x25,
    0x13, 0x88, 0x7f, 0xe4, 0xd2, 0x49, 0x95, 0x0e, 0x38, 0xa3, 0x54, 0xcf, 0xf9, 
    0x62, 0x8c, 0x17, 0x21, 0xba, 0x4d, 0xd6, 0xe0, 0x7b
};


// ---------------
// Kermit Protocol
// ---------------

/*
    @brief Init the package with the type of package

    @param pckg (package_t &) : The reference of package
    @param type (uint32_t) : The type of package

    @return void
*/
void initPackage(package_t &pckg, uint32_t type);


// -----------------
// Data Manipulation
// -----------------

/*
    @brief Divides the data sent into an array of packages

    @param data (const void *) : An array with the data that is going to be sent
    @param size (uint32_t)     : The size of the array data
    @param type (uint32_t)     : The type of the packages to be created
    @param seq  (uint32_t)     : The desired sequence number of the starting 
    package
    @param fName (const std::string &)    : The name of the file to be sent
    @param uName (const std::string &)    : The username of the client

    @return Returns an array of packages containing the data sent, the last 
    element will have type END_PACKAGE. Returns NULL if there is an error
*/
package_t * divideData(const void *data, uint32_t size, uint32_t type, 
uint32_t seq, const std::string &fName = "", const std::string &uName = "Drey Drey");

/*
    @brief Combines data from an array of packages into an array of voids
    
    @param packs (package_t *) : An array of packages, first package with data, 
    last package should have type END_PACKAGE

    @return Returns an array of the data that was contained in the packages plus a '\0' at the end
*/
char *combineData(package_t * packs);

/*
    @brief Masks the data inside package and regenerates crc

    @param pckg (package_t *) : The package to be masked

    @return void
*/
void maskPackage(package_t * pckg);


// ---------------
// Print Functions
// ---------------

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


// ---------------------
// CRC8 WCDMA Calcultion
// ---------------------

/*
    @brief Validate the CRC of the package with the a new CRC generated with the
     content of the package 

    @param pckg (const package_t &) : The reference of package

    @return Returns true if the CRC is valid, false otherwise
*/
bool validateCRC(const package_t &pckg);

/*
    @brief Generate the CRC of the package using data and length of the data

    @param data (char *) : The data of the package
    @param len  (size_t) : The length of the data

    @return Returns the CRC of the package
*/
unsigned char generateCRC(char *data, size_t len);

/*
    @brief Generate the CRC of the package

    @param pckg (const package_t &) : The reference of package

    @return Returns the CRC of the package
*/
unsigned char generateCRC(const package_t &pckg);

/*
    @brief Reflect the data for CRC calculation

    @param data (const unsigned char) : The data to be reflected

    @return Returns the reflected data
*/
unsigned char reflectData(const unsigned char &data);


// --------------
// Socket Timeout
// --------------

/*
    @brief Sets the time to wait for receiving/sending on a socket

    @param sckt (cont int &) : socket to set the timeout timer
    @param useconds (const int) : time in microseconds to set the timeout (1.000.000=1 second)

    @return void
*/
void setSockTimeout(const int &sckt, const int useconds);


#endif
