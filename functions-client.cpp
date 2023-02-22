#include "functions-client.hpp"

// --------------------------
// Internal functions headers
// --------------------------

/*
    @brief Waits for Ack/Nack

    @param sckt (const int &) : The File Descriptor of the socket
    @param pckg (package_t *) : The package that will be received

    @return -1 if timed out
    @return 0 on success
*/
int waitResponse( const int &sckt, package_t * pckg );


// ---------------
// Implementations
// ---------------

int sendMessage(const int &sckt, const int seq){
    char ch {};
    std::string message {};
    
    while ( (ch = (char) std::cin.get()) != STOP_INSERT_COMMAND ) message += ch;

    package_t * packs {divideData(
                      (void *)message.c_str(),
                      message.size(),
                      TEXT_PACKAGE,
                      (seq+1)%16)};

    unsigned int numSent = sendPacks(sckt, packs);
    if ( !numSent ) {
         std::cerr << "Error sending message" << std::endl;
         std::cerr << std::endl;
         std::cerr << errno << std::endl;
         return -1;
    }
    std::cerr << "Successful message sent" << std::endl;
    std::cerr << std::endl;
    return packs[numSent-1].seq;
}


void sendFile(const int &sckt){
    std::string path;
    std::cin >> path;

    std::cerr << "Arquivo a ser enviado: " << path << std::endl;
    std::cerr << std::endl;
}

unsigned int sendPacks(const int &sckt, package_t * packs){
  const int windowSize {5};
  unsigned int currPackIndex {0};

  // send start package to stabilish connection
  package_t start {};
  initPackage(start, START_PACKAGE);
  start.seq = (packs[0].seq - 1) % 16;

  unsigned int timeoutCounter {0};
  package_t response {};
  while ( timeoutCounter < 20 ){
    ssize_t ret {send(sckt, &start, sizeof(package_t), 0)}; 
    if ( ret < 0 ){
        std::cerr << "Error sending packages" << std::endl;
        std::cerr << std::endl;
        std::cerr << errno << std::endl;
        return 0;
    }
    int hadResponse {waitResponse(sckt, &response)};
    if ( hadResponse == 0 && response.type == ACK_PACKAGE && response.seq == start.seq ) break;
    timeoutCounter++;
  }
  if (timeoutCounter >= 20) return 0;
  std::cerr << "Estabilished connection" << std::endl;

  // while there are still packs to send
  bool done = false;
  while ( !done ){
    // send packages for current window
    for ( unsigned int i {0}; i < windowSize; i++){
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

    response.type = START_PACKAGE;
    int hadResponse {0};
    int lastOK {-1};
    bool gotNack = false;
    // deal with errors from loopback
    while ( (lastOK < packs[currPackIndex].seq && packs[currPackIndex].seq - lastOK < 16 - windowSize) ||
            (lastOK > packs[currPackIndex].seq && lastOK - packs[currPackIndex].seq > windowSize + 1)  ||
            (response.type != ACK_PACKAGE && response.type != NACK_PACKAGE) ){
      hadResponse = waitResponse(sckt, &response);
      if ( hadResponse == 0 ) lastOK = response.seq;
      if ( response.type == NACK_PACKAGE ){
        //if package valid from loopback
        if ( (lastOK < packs[currPackIndex].seq && packs[currPackIndex].seq - lastOK < 16 - windowSize) ||
             (lastOK > packs[currPackIndex].seq && lastOK - packs[currPackIndex].seq > windowSize + 1) ){
          lastOK = (lastOK - 1) % 16;
          gotNack = true;
        }
      }
    }
    std::cerr << "Starting index is  " << currPackIndex << std::endl;
    std::cerr << "Sent starting from " << packs[currPackIndex].seq << std::endl;
    std::cerr << "Recieved till      " << lastOK << std::endl;
    std::cerr << "Diference is       " << lastOK - packs[currPackIndex].seq << std::endl;
    if ( hadResponse == -1 ) continue;


    // if the seq number would surpass 15
    if ( packs[currPackIndex].seq - lastOK > windowSize ) {
      currPackIndex += lastOK + 16 - packs[currPackIndex].seq + 1;
    // generic case
    } else {
      currPackIndex += lastOK - packs[currPackIndex].seq + 1;
    }

    // if sent all packages
    if ( packs[currPackIndex-1].type == END_PACKAGE ) done = true;
    std::cerr << "Next start at      " << packs[currPackIndex].seq << std::endl;
    std::cerr << "Next index is      " << currPackIndex << std::endl << std::endl;
  }
  return currPackIndex;
}

void quitProgram(bool &stop){
    stop = true;
    std::cout << std::endl;
    std::cout << "Bye! | Tchau! | Tchüss!" << std::endl;
    std::cout << std::endl;
}


// Internal functions

int waitResponse( const int &sckt, package_t * pckg ){
    do{
      ssize_t val {recv(sckt, pckg, sizeof(pckg), 0)};
      if ( val == -1 && ( errno == EAGAIN)){
        std::cerr << "Timed out " << EAGAIN << std::endl;
        return -1;
      }

      //printPackage(pckg);
      if (pckg->type == ACK_PACKAGE || pckg->type == NACK_PACKAGE ){
          std::cerr << "(n)ack package:" << std::endl;
          printPackage(*pckg);
      }
      return 0;
    } while ( true );
}
