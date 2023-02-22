
#include "functions-server.hpp"


void receivePackage(const int &sckt, package_t &pckg){
    ssize_t val {recv(sckt, &pckg, sizeof(pckg), 0)};

    if ( val == -1 && ( errno == EAGAIN ) ){
        return;
    }

    if ( val >= 0 ){
        handlePacks(sckt, &pckg);
    }
    // else {
    //     std::cerr << "Error receiving message" << std::endl;
    //     printPackage(pckg);
    // }
}


package_t * receiveOtherPacks(const int &sckt, uint32_t seq){
    package_t * packs {(package_t *)malloc(64 * sizeof(package_t))};
    uint32_t lastRecv {0};
    uint32_t pckgArraySize {64};

    package_t pckg {0};
    const uint32_t windowSize {5};

    for (;;){
        std::array<package_t, windowSize> receiving {0};
        std::array<bool, windowSize> oks {};
    
        for ( uint32_t i {0}; i < windowSize; i++ ) oks[i] = false;
        for ( uint32_t i {0}; i < windowSize; i++){
            ssize_t val {recv(sckt, &pckg, sizeof(pckg), 0)};
        
            //If timeout
            if ( val == -1 && ( errno == EAGAIN ) ){ break; }
        
            //CRC validation
            if ( !validateCRC(pckg) ) { continue; }
        
            //If duplicate package from loopback
            if ( (pckg.seq < seq && seq - pckg.seq < 16 - windowSize ) ||
                (pckg.seq > seq && pckg.seq - seq > windowSize) ||
                (pckg.type == ACK_PACKAGE || pckg.type == NACK_PACKAGE)){
                std::cerr << "Duplicate from LoopBack" << std::endl;
                std::cerr << seq << std::endl;
                std::cerr << pckg.seq << std::endl;
                i--;
                continue;
            }

            //Find where to put package in the temporary array
            int difference {0};
            difference = pckg.seq - seq;

            //If sequence number supassed 15
            if ( difference < 0 ){
                difference = pckg.seq + 16 - seq;
            }

            //Check if already had that package
            if ( oks[difference] ){
                std::cerr << "Got old package: " << difference << std::endl;
                i--;
                continue;
            }

            //Save otherwise
            std::cerr << "Got new package" << std::endl;
            
            printPackage(pckg);
            
            receiving[difference] = pckg;
            oks[difference] = true;
            if (pckg.type == END_PACKAGE) break;
        }
        
        for ( uint32_t i {0}; i < windowSize; i++ ){
            std::cerr << oks[i] << " ";
        }
        std::cerr << std::endl;

        //If couldnt receive first one, just time out
        if ( !oks[0] ) continue;

        // Copy good receiveds to permanent
        std::cerr << "Copying good receives" << std::endl;
        
        for ( uint32_t i {0}; i < windowSize && oks[i]; i++ ){
            packs[lastRecv++] = receiving[i];
            seq = (seq + 1) % 16;
            if (lastRecv >= pckgArraySize) {
                pckgArraySize += 64;
                packs = (package_t*)realloc(packs, pckgArraySize * sizeof(package_t));
            }
        }
        
        package_t ack {0};

        initPackage(ack, ACK_PACKAGE);
        ack.seq = (seq - 1) % 16;

        std::cerr << "Sending ack for sliding window: " << seq << std::endl 
            << std::endl 
            << std::endl 
            << std::endl 
            << std::endl 
            << std::endl 
            << std::endl;

        ssize_t ret {send(sckt, &ack, sizeof(package_t), 0)}; 

        if ( ret < 0 ){
            std::cerr << "Error sending ack for sliding window" << std::endl;
            std::cerr << std::endl;
            std::cerr << errno << std::endl;
            return NULL;
        }

        if ( packs[lastRecv-1].type == END_PACKAGE ) {
            std::cerr << "Received all packages" << std::endl;
            break;
        }
    }

    return packs;
}


void handlePacks(const int &sckt, package_t *packs){
    switch (packs[0].type){
        case TEXT_PACKAGE:
            handleMessages(packs);
        break;
        case MEDIA_PACKAGE:
            handleMedias(packs);
        break;
        // case ACK_PACKAGE:
        //   break;
        // case NACK_PACKAGE:
        //   break;
        // case ERROR_PACKAGE:
        //   break;
        case START_PACKAGE:
        {
            // send ack for first package
            package_t ack {0};
            initPackage(ack, ACK_PACKAGE);
            ack.seq = packs[0].seq;
            ssize_t ret {send(sckt, &ack, sizeof(package_t), 0)}; 
            
            if ( ret < 0 ){
                std::cerr << "Error sending ack for start" << std::endl;
                std::cerr << std::endl;
                std::cerr << errno << std::endl;
                return;
            }

            std::cerr << "First sequence to receive: " << packs[0].seq + 1 << std::endl;
            package_t *newPacks {receiveOtherPacks(sckt, packs[0].seq + 1)};
            handlePacks(sckt, newPacks);

            free(newPacks);
        }
        break;
        default:
        break;
    }
}


void handleMessages(package_t *packs){
    char *data {combineData(packs)};

    printDate();
    std::cout << data << std::endl << std::endl;

    delete data;
}


void handleMedias(package_t *packs){
    std::string saveDir {"./receivedFiles/"};
    if ( !std::filesystem::exists(saveDir) ){
        std::filesystem::create_directory(saveDir);
    }

    uint32_t size {0};
    for (; packs[size].type != END_PACKAGE; size++);

    std::string username {(char *) packs[size - 2].data};
    std::string fileName {(char *) packs[size - 1].data};
    std::string path {saveDir + fileName};
    std::ofstream file {path, std::ios::binary}; 

    for (uint32_t curr {0}; curr != size - 2; curr++){ 
        file.write((char *) packs[curr].data, packs[curr].size);
    }
    
    file.close();

    printDate();
    std::cout << username << " : " 
        << "Arquivo de midia enviado! O arquivo pode ser encontrado em " 
        << path
        << std::endl << std::endl;
}
