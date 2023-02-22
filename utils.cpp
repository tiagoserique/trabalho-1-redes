
#include "utils.hpp"


// ---------------
// Kermit Protocol
// ---------------

void initPackage(package_t &pckg, uint32_t type){
    pckg.header   = PACKAGE_START_MARK;
    pckg.type     = type;
    pckg.seq      = 0;
    pckg.size     = 0;
    pckg.data[0]  = '\0';
    pckg.crc      = 0x00;
}


// -----------------------
// CRC 8 WCDMA Calculation
// -----------------------

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


// -----------------
// Data Manipulation
// -----------------

package_t *divideData(const void * data, uint32_t size, uint32_t type, 
uint32_t seq, const std::string &fName, const std::string &uName){
    if ( !data ) return NULL;
    if ( size <= 0 ) return NULL;

    uint32_t numPacks {size/63};
    if ( size % 63 ) numPacks++;
    char * cdata {(char *) data};

    package_t * packs {nullptr};

    if ( type == MEDIA_PACKAGE ){
        if ( fName.empty() ) return NULL;
        if ( uName.empty() ) return NULL;

        packs = new package_t[numPacks + 3];
    }
    else 
        packs = new package_t[numPacks + 1];


    uint32_t dataIndex {0};
    for ( uint32_t i = 0; i < numPacks; i++ ){
        initPackage(packs[i], type);
        packs[i].seq = seq;
        seq = (seq + 1) % 16;

        // copy data
        uint32_t j {0};
        while ( j < 63 && dataIndex < size ){
            packs[i].data[j] = cdata[dataIndex];
            dataIndex++; j++;
        }
        packs[i].size = j;
        packs[i].crc = generateCRC(packs[i]);
    }

    if ( type == MEDIA_PACKAGE ){
        // Copy username
        initPackage(packs[numPacks], type);
        packs[numPacks].seq = seq;
        seq = (seq + 1) % 16;

        uint32_t j {0};
        while ( j < 63 && j <= uName.size() ){
            packs[numPacks].data[j] = uName[j];
            j++;
        }
        packs[numPacks].size = j;
        packs[numPacks].crc = generateCRC(packs[numPacks]);

        numPacks++;

        // Copy filename
        initPackage(packs[numPacks], type);
        packs[numPacks].seq = seq;
        seq = (seq + 1) % 16;

        j = 0;
        while ( j < 63 && j <= fName.size() ){
            packs[numPacks].data[j] = fName[j];
            j++;
        }
        packs[numPacks].size = j;
        packs[numPacks].crc = generateCRC(packs[numPacks]);

        numPacks++;
    }

    initPackage(packs[numPacks], END_PACKAGE);
    packs[numPacks].seq = seq;

    return packs;
}

char *combineData(package_t * packs){
    // Get size of data to copy
    uint32_t size {0};
    for (uint32_t curr {0}; packs[curr].type != END_PACKAGE; curr++) size++;
    char * result {new char[(size * 64) + 1]};

    uint32_t resultIndex {0};

    // Copy the data
    for (uint32_t curr {0}; packs[curr].type != END_PACKAGE; curr++){
        uint32_t currCopy {0};
        while ( currCopy < packs[curr].size ){
            result[resultIndex++] = packs[curr].data[currCopy++];
        }
    }

    result[resultIndex] = '\0';
    
    return result;
}


// -----------------
// Print Functions
// -----------------

void printPackage(const package_t &pckg){
    std::cerr << pckg.header   << std::endl;
    std::cerr << pckg.type     << std::endl;
    std::cerr << pckg.seq      << std::endl;
    std::cerr << pckg.size     << std::endl;
    std::cerr << pckg.data     << std::endl;
    std::cerr << pckg.crc      << std::endl;
    std::cerr << std::endl;
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


// --------------
// Socket Timeout
// --------------

void setSockTimeout(const int &sckt, const int useconds){
    struct timeval tv {0};
    tv.tv_usec = useconds;
    if (setsockopt(sckt, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) 
        std::cerr << "setsockopt failed\n";
    
    if (setsockopt(sckt, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0)
        std::cerr << "setsockopt failed\n";
}

