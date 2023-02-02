
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


// crc 8 wcdma calculation =====================================================

bool validateCRC(const package_t &pckg){
    unsigned char crc {generateCRC(pckg)};
    
    if ( crc != pckg.crc ){
        std::cerr << "Error in CRC validation "
            << "| Difference between package CRC and calculated CRC" 
            << std::endl;
        
        return false;
    }

    std::cerr << "CRC validation successful" << std::endl;

    return true;
}

unsigned char generateCRC(char *data, size_t len){
    unsigned char crc {0x00};

    for (size_t i {0}; i < len; i++) {
        crc = crc8x_table[(crc ^ reflectData(data[i]))];
    }

    return reflectData(crc);
}

unsigned char generateCRC(const package_t &pckg){
    unsigned char crc = 0x00;

    for (ssize_t i {0}; i < pckg.size; i++){
        crc = crc8x_table[crc ^ reflectData(pckg.data[i])];
    }

    return reflectData(crc);
}

unsigned char reflectData(const unsigned char &data){
    unsigned char reflected {0x00};

    for (size_t i {0}; i < 8; i++){
        if ( (data & (1 << i)) != 0 ){
            reflected |= (unsigned char)(1 << (7 - i));
        }
    }

    return reflected;
}


// utils =======================================================================

package_t * divideData(void * data, unsigned int size, unsigned int type, unsigned int seq){
    if ( !data ) return NULL;
    if ( size <= 0 ) return NULL;

    unsigned int numPacks {(size/63)+1};
    if ( size % 63 ) numPacks++;
    char * cdata {(char *) data};

    package_t * packs {(package_t *) malloc(sizeof(package_t) * numPacks + 1)};
    if ( !packs ) return NULL;

    unsigned int dataIndex {0};
    for ( unsigned int i = 0; i < numPacks; i++ ){
        initPackage(packs[i], type);
        packs[i].sequence = seq;
        seq = (seq + 1) % 16;

        // copy data
        unsigned int j {0};
        while ( j < 63 && dataIndex < size ){
          packs[i].data[j] = cdata[dataIndex];
          dataIndex++; j++;
        }
        packs[i].size = j;
        packs[i].crc = generateCRC(packs[i]);
    }
    initPackage(packs[numPacks], END_PACKAGE);
    packs[numPacks].sequence = seq;

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

void setSockTimeout(const int &sckt, const int useconds){
    struct timeval tv {0};
    tv.tv_usec = useconds;
    if (setsockopt(sckt, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) 
        std::cerr << "setsockopt failed\n";
    
    if (setsockopt(sckt, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0)
        std::cerr << "setsockopt failed\n";
}
