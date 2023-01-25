
#include "utils.hpp"


// kermit protocol =============================================================

void initPackage(package_t &pckg, unsigned int type){
    pckg.header   = PACKAGE_START_MARK;
    pckg.type     = type;
    pckg.sequence = 1;
    pckg.size     = 0;
    pckg.data[0]  = '\0';
    pckg.crc      = 0x00;
}


// utils =======================================================================

package_t * divideData(void * data, unsigned int size, unsigned int type, unsigned int seq){
    if ( !data ) return NULL;
    if ( size <= 0 ) return NULL;

    unsigned int numPacks {size/63};
    if ( size % 63 ) numPacks++;
    char * cdata {(char *) data};

    package_t * packs {(package_t *) malloc(sizeof(package_t) * numPacks + 1)};
    if ( !packs ) return NULL;

    unsigned int dataIndex {0};
    for ( unsigned int i = 0; i < numPacks; i++ ){
        initPackage(packs[i], type);
        packs[i].sequence = seq;
        seq = (seq + 1) % 16;

        // copiar dados
        unsigned int j {0};
        while ( j < 63 && dataIndex < size ){
          packs[i].data[j] = cdata[dataIndex];
          dataIndex++; j++;
        }
        packs[i].size = j;
    }
    initPackage(packs[numPacks], END_PACKAGE);

    return packs;
}

void * combineData(package_t * packs){
    // Get size of data to copy
    unsigned int size {0};
    for (unsigned int curr {0}; packs[curr].type != END_PACKAGE; curr++) size++;
    char * result {(char *)malloc( (size * 64) + 1 )};

    unsigned int resultIndex {0};

    // Copy the data
    for ( unsigned int currPack {0}; packs[currPack].type != END_PACKAGE; currPack++ ){
        unsigned int currCopy {0};
        while ( currCopy < packs[currPack].size ){
            result[resultIndex++] = packs[currPack].data[currCopy++];
        }
    }
    result[resultIndex] = '\0';
    return (void *)result;
}

void printPackage(const package_t &pckg){
    std::cout << pckg.header   << std::endl;
    std::cout << pckg.type     << std::endl;
    std::cout << pckg.sequence << std::endl;
    std::cout << pckg.size     << std::endl;
    std::cout << pckg.data     << std::endl;
    std::cout << pckg.crc      << std::endl;
    std::cout << std::endl;
}


void printDate(){
    time_t now = time(0);
    tm *ltm = localtime(&now);

    // date variables
    int day   = ltm->tm_mday;
    int month = 1 + ltm->tm_mon;
    int year  = 1900 + ltm->tm_year;

    // time variables
    int hour   = ltm->tm_hour;
    int minute = ltm->tm_min;
    int second = ltm->tm_sec;

    std::cout << "["
        << day << "/" << month << "/" << year
        << " - "
        << hour << ":" << minute << ":" << second 
        << "]";
}

// A tabela de navegação do crc
static char const crc8x_table[] = {
    0x00, 0x9b, 0xad, 0x36, 0xc1, 0x5a, 0x6c, 0xf7, 0x19, 0x82, 0xb4, 0x2f, 0xd8, 0x43, 0x75, 0xee, 0x32, 0xa9, 0x9f, 0x04, 0xf3, 0x68, 0x5e, 0xc5, 0x2b, 0xb0, 0x86, 0x1d, 0xea, 0x71, 0x47, 0xdc, 0x64, 0xff, 0xc9, 0x52, 0xa5, 0x3e, 0x08, 0x93, 0x7d, 0xe6, 0xd0, 0x4b, 0xbc, 0x27, 0x11, 0x8a, 0x56, 0xcd, 0xfb, 0x60, 0x97, 0x0c, 0x3a, 0xa1, 0x4f, 0xd4, 0xe2, 0x79, 0x8e, 0x15, 0x23, 0xb8, 0xc8, 0x53, 0x65, 0xfe, 0x09, 0x92, 0xa4, 0x3f, 0xd1, 0x4a, 0x7c, 0xe7, 0x10, 0x8b, 0xbd, 0x26, 0xfa, 0x61, 0x57, 0xcc, 0x3b, 0xa0, 0x96, 0x0d, 0xe3, 0x78, 0x4e, 0xd5, 0x22, 0xb9, 0x8f, 0x14, 0xac, 0x37, 0x01, 0x9a, 0x6d, 0xf6, 0xc0, 0x5b, 0xb5, 0x2e, 0x18, 0x83, 0x74, 0xef, 0xd9, 0x42, 0x9e, 0x05, 0x33, 0xa8, 0x5f, 0xc4, 0xf2, 0x69, 0x87, 0x1c, 0x2a, 0xb1, 0x46, 0xdd, 0xeb, 0x70, 0x0b, 0x90, 0xa6, 0x3d, 0xca, 0x51, 0x67, 0xfc, 0x12, 0x89, 0xbf, 0x24, 0xd3, 0x48, 0x7e, 0xe5, 0x39, 0xa2, 0x94, 0x0f, 0xf8, 0x63, 0x55, 0xce, 0x20, 0xbb, 0x8d, 0x16, 0xe1, 0x7a, 0x4c, 0xd7, 0x6f, 0xf4, 0xc2, 0x59, 0xae, 0x35, 0x03, 0x98, 0x76, 0xed, 0xdb, 0x40, 0xb7, 0x2c, 0x1a, 0x81, 0x5d, 0xc6, 0xf0, 0x6b, 0x9c, 0x07, 0x31, 0xaa, 0x44, 0xdf, 0xe9, 0x72, 0x85, 0x1e, 0x28, 0xb3, 0xc3, 0x58, 0x6e, 0xf5, 0x02, 0x99, 0xaf, 0x34, 0xda, 0x41, 0x77, 0xec, 0x1b, 0x80, 0xb6, 0x2d, 0xf1, 0x6a, 0x5c, 0xc7, 0x30, 0xab, 0x9d, 0x06, 0xe8, 0x73, 0x45, 0xde, 0x29, 0xb2, 0x84, 0x1f, 0xa7, 0x3c, 0x0a, 0x91, 0x66, 0xfd, 0xcb, 0x50, 0xbe, 0x25, 0x13, 0x88, 0x7f, 0xe4, 0xd2, 0x49, 0x95, 0x0e, 0x38, 0xa3, 0x54, 0xcf, 0xf9, 0x62, 0x8c, 0x17, 0x21, 0xba, 0x4d, 0xd6, 0xe0, 0x7b
};

// Recebe array de dados e seu tamanho, retorna o crc 8bits
char genCrc(char *data, size_t len)
{
    char crc = 0xff;
    for (int i = 0; i < len; i++) {
        crc = crc8x_table[crc ^ data[i]];
    }
    return crc;
}

//// Ou a versão q recebe um package (não testada pq eu n consegui rodar)

// char genCrc(const package_t &pckg)
// {
//     char crc = 0xff;
//     for (int i = 0; i < pckg.size; i++) {
//         crc = crc8x_table[crc ^ pckg.data[i]];
//     }
//     return crc;
// }