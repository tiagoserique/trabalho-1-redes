
#include "functions-server.hpp"


void receivePackage(const int &sckt, package_t &pckg){
    ssize_t val {recv(sckt, &pckg, sizeof(pckg), 0)};

    if ( val == -1 && ( errno == EAGAIN ) ){
        return;
    }

    if ( val >= 0 && pckg.type == START_PACKAGE ){
        //send ack for first package
        package_t ack {0};
        initPackage(ack, ACK_PACKAGE);
        ack.seq = pckg.seq;
        ssize_t ret {send(sckt, &ack, sizeof(package_t), 0)}; 
        
        if ( ret < 0 ){
            std::cerr << "Error sending ack for start" << std::endl;
            std::cerr << std::endl;
            std::cerr << errno << std::endl;
            return;
        }

        std::cerr << "First sequence to receive: " << pckg.seq + 1 << std::endl;
        package_t *packs {receiveOtherPacks(sckt, pckg.seq + 1)};
        handlePacks(packs);
    }
    // else {
    //     std::cerr << "Error receiving message" << std::endl;
    //     printPackage(pckg);
    // }
}


package_t * receiveOtherPacks(const int &sckt, unsigned int seq){
    package_t * packs {(package_t *)malloc(64 * sizeof(package_t))};
    unsigned int lastRecv {0};
    unsigned int pckgArraySize {64};

    package_t pckg {0};
    const unsigned int windowSize {5};

    for (;;){
        std::array<package_t, windowSize> receiving {0};
        std::array<bool, windowSize> oks {};
    
        for ( unsigned int i {0}; i < windowSize; i++ ) oks[i] = false;
        for ( unsigned int i {0}; i < windowSize; i++){
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
        
        for ( unsigned int i {0}; i < windowSize; i++ ){
            std::cerr << oks[i] << " ";
        }
        std::cerr << std::endl;

        //If couldnt receive first one, just time out
        if ( !oks[0] ) continue;

        // Copy good receiveds to permanent
        std::cerr << "Copying good receives" << std::endl;
        
        for ( unsigned int i {0}; i < windowSize && oks[i]; i++ ){
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


void handlePacks(package_t * packs){
    void * data = combineData(packs);

    switch (packs[0].type){
        case TEXT_PACKAGE:
            handleMessages((char*) data);
            break;
        case MEDIA_PACKAGE:
            handleMedias((char*) data);
            break;
        // case ACK_PACKAGE:
        //   break;
        // case NACK_PACKAGE:
        //   break;
        // case ERROR_PACKAGE:
        //   break;
        // case START_PACKAGE:
        //   break;
        // case END_PACKAGE:
        //   break;
        // case DATA_PACKAGE:
        //   break;
        // default:
        //   break;
    }
}


void handleMessages(char * data){

    // message's format
    // [<date e hour>]<User> : message
    
    printDate();
    std::cout << "<Usuário> : " << data << std::endl << std::endl;
}


void handleMedias(char *data){
    // File message's format
    /* [<data e hour>]<User> : Arquivo de midia enviado! O arquivo pode ser 
        encontrado em <caminho_para_arquivo>
    */

    std::string path {"./receivedFiles/"};

    if ( !std::filesystem::exists(path) ){
        std::filesystem::create_directory(path);
    }

    std::string fileName {path + "file"};
    std::ofstream file {fileName, std::ios::binary};


    file.write(data, sizeof(data));

    file.close();

    printDate();
    std::cout << "<Usuário> : " 
        << "Arquivo de midia enviado! O arquivo pode ser encontrado em " 
        << fileName
        << std::endl << std::endl;
}
