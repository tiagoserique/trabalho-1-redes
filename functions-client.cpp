#include "functions-client.hpp"


// ---------------
// Implementations
// ---------------

int sendMessage(const int &sckt, const int &seq, const std::string &username){
    char ch {};
    std::string message {username + " : "};
    
    while ( (ch = (char) std::cin.get()) != STOP_INSERT_COMMAND ) message += ch;

    package_t * packs {
        divideData(
            (void *)message.c_str(), 
            message.size(), 
            TEXT_PACKAGE, 
            (seq + 1) % 16
        )
    };

    uint32_t numSent {sendPacks(sckt, packs)};
    if ( !numSent ) {
        std::cerr << "Error sending message" << std::endl;
        std::cerr << std::endl;
        std::cerr << errno << std::endl;
        return -1;
    }

    std::cerr << "Successful message sent" << std::endl;
    std::cerr << std::endl;

    uint32_t ret {packs[numSent - 1].seq};

    delete packs;
    return ret;
}


int sendFile(const int &sckt, const int &seq, const std::string &username){
    std::string path;
    
    std::cout << "Enter file path: ";
    std::cin >> path;

    std::string fileName {path.substr(path.find_last_of("/") + 1)};

    std::ifstream file {path, std::ios::binary};
    if ( !file.is_open() ){
        std::cerr << "Error opening file" << std::endl;
        std::cerr << std::endl;
        return -1;
    }

    file.seekg(0, std::ios::end);
    std::streampos fileSize {file.tellg()};
    file.seekg(0, std::ios::beg);

    char *buffer {new char[fileSize]};
    file.read(buffer, fileSize);
    file.close();

    package_t *packs {
        divideData(
            (void *)buffer, 
            fileSize, 
            MEDIA_PACKAGE, 
            (seq + 1) % 16,
            fileName,
            username
        )
    };

    uint32_t numSent {sendPacks(sckt, packs)};

    if ( !numSent ) {
        std::cerr << "Error sending file" << std::endl;
        std::cerr << std::endl;
        std::cerr << errno << std::endl;
        return -1;
    }
    std::cerr << "Successful file sent" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Number of packages sent: " << numSent << std::endl;

    uint32_t ret {packs[numSent - 1].seq};

    delete packs;
    delete buffer;

    return ret;
}


void quitProgram(bool &stop){
    stop = true;
    std::cout << std::endl;
    std::cout << "Bye! | Tchau! | Tchüss!" << std::endl;
    std::cout << std::endl;
}


// --------------------------
// Internal functions headers
// --------------------------

int waitResponse(const int &sckt, const uint32_t type){
    package_t ack {};

    do {
        ssize_t val {recv(sckt, &ack, sizeof(ack), 0)};
        if ( val == -1 && ( errno == EAGAIN)){
            std::cerr << "Timed out " << EAGAIN << std::endl;
            return -1;
        }

        if ( ack.type != type ){
            // TODO: Save somewhere
            continue;
        }

        //printPackage(ack);
        std::cerr << "Ack package:" << std::endl;
        printPackage(ack);
      
        return ack.seq;
    } while ( true );
}


uint32_t sendPacks(const int &sckt, package_t * packs){
    const int windowSize {5};
    uint32_t currPackIndex {0};

    // send start package to stabilish connection
    package_t start {};
    initPackage(start, START_PACKAGE);
    start.seq = (packs[0].seq - 1) % 16;

    uint32_t timeoutCounter {0};
    while ( waitResponse(sckt, ACK_PACKAGE) != start.seq && timeoutCounter < 20){
        ssize_t ret {send(sckt, &start, sizeof(package_t), 0)}; 
        if ( ret < 0 ){
            std::cerr << "Error sending packages" << std::endl;
            std::cerr << std::endl;
            std::cerr << errno << std::endl;
            return 0;
        }
        timeoutCounter++;
    }
    
    if (timeoutCounter >= 20) return 0;
    std::cerr << "Estabilished connection" << std::endl;

    // while there are still packs to send
    bool done = false;
    while ( !done ){
        // send packages for current window
        for (uint32_t i {0}; i < windowSize; i++){
            ssize_t ret {send(sckt, &(packs[currPackIndex+i]), sizeof(package_t), 0)}; 
            if ( ret < 0 ){
                std::cerr << "Error sending packages" << std::endl;
                std::cerr << std::endl;
                std::cerr << errno << std::endl;
                return 0;
            }
            
            if ( packs[currPackIndex+i].type == END_PACKAGE ) {
                std::cerr << "sending last package" << std::endl;
                break;
            }
        }

        int lastOK {waitResponse(sckt, ACK_PACKAGE)};
        
        // deal with errors from loopback
        while ( (lastOK < packs[currPackIndex].seq && packs[currPackIndex].seq - lastOK < 16 - windowSize) ||
                (lastOK > packs[currPackIndex].seq && lastOK - packs[currPackIndex].seq > windowSize + 1) ){
            
            if ( lastOK == -1 ) {
                break;
            }
            
            lastOK = waitResponse(sckt, ACK_PACKAGE);
        }
        std::cerr << "Starting index is  " << currPackIndex << std::endl;
        std::cerr << "Sent starting from " << packs[currPackIndex].seq << std::endl;
        std::cerr << "Recieved till      " << lastOK << std::endl;
        std::cerr << "Diference is       " << lastOK - packs[currPackIndex].seq << std::endl;
        if ( lastOK == -1 ) continue;


        // if the seq number would surpass 15
        if ( packs[currPackIndex].seq - lastOK > windowSize ) {
            currPackIndex += lastOK + 16 - packs[currPackIndex].seq + 1;
            // generic case
        } 
        else {
            currPackIndex += lastOK - packs[currPackIndex].seq + 1;
        }

        // if sent all packages
        if ( packs[currPackIndex-1].type == END_PACKAGE ) done = true;
        std::cerr << "Next start at      " << packs[currPackIndex].seq << std::endl;
        std::cerr << "Next index is      " << currPackIndex << std::endl << std::endl;
    }

    return currPackIndex;
}
