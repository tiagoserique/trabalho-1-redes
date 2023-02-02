#include "functions-client.hpp"

// --------------------------
// Internal functions headers
// --------------------------

/*
    @brief Waits for Ack/Nack

    @param sckt (const int &) : The File Descriptor of the socket
    @param type (const int) : The type of package to wait

    @return -1 if timed out
    @return The sequence number of the ack package otherwise
*/
int waitResponse( const int &sckt, const unsigned int type );


// ---------------
// Implementations
// ---------------

void sendMessage(const int &sckt){
    char ch {};
    std::string message {};
    
    while ( (ch = (char) std::cin.get()) != STOP_INSERT_COMMAND ) message += ch;

    package_t * packs {divideData(
                      (void *)message.c_str(),
                      message.size(),
                      TEXT_PACKAGE,
                      10)};

    unsigned int numSent = sendPacks(sckt, packs);
    if ( !numSent ) {
         std::cerr << "Error sending message" << std::endl;
         std::cerr << std::endl;
         std::cerr << errno << std::endl;
         return;
    }
    std::cerr << "Successful message sent" << std::endl;
    std::cerr << std::endl;
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
  start.sequence = (packs[0].sequence - 1) % 16;

  unsigned int timeoutCounter {0};
  while ( waitResponse(sckt, ACK_PACKAGE) != start.sequence && timeoutCounter < 20){
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
  while ( packs[currPackIndex].type != END_PACKAGE ){
    // send packages for current window
    for ( unsigned int i {0}; i < windowSize; i++){
      ssize_t ret {send(sckt, &(packs[currPackIndex+i]), sizeof(package_t), 0)}; 
      if ( ret < 0 ){
          std::cerr << "Error sending packages" << std::endl;
          std::cerr << std::endl;
          std::cerr << errno << std::endl;
          return 0;
      }
      if ( packs[currPackIndex+i].type == END_PACKAGE ) break;
    }
    int lastOK {waitResponse(sckt, ACK_PACKAGE)};
    // deal with errors from loopback
    while ( (lastOK < packs[currPackIndex].sequence && packs[currPackIndex].sequence - lastOK < 16 - windowSize) ||
            (lastOK > packs[currPackIndex].sequence && lastOK - packs[currPackIndex].sequence > windowSize + 1) ){
      if ( lastOK == -1 ) {
        break;
      }
      lastOK = waitResponse(sckt, ACK_PACKAGE);
    }
    std::cerr << "Starting index is  " << currPackIndex << std::endl;
    std::cerr << "Sent starting from " << packs[currPackIndex].sequence << std::endl;
    std::cerr << "Recieved till      " << lastOK << std::endl;
    std::cerr << "Diference is       " << lastOK - packs[currPackIndex].sequence << std::endl;
    if ( lastOK == -1 ) continue;


    // if the sequence number would surpass 15
    if ( packs[currPackIndex].sequence - lastOK > windowSize ) {
      currPackIndex += lastOK + 16 - packs[currPackIndex].sequence + 1;
    // generic case
    } else {
      currPackIndex += lastOK - packs[currPackIndex].sequence + 1;
    }

    // if sent all packages
    if ( packs[currPackIndex-1].type == END_PACKAGE ) break;
    std::cerr << "Next start at      " << packs[currPackIndex].sequence << std::endl;
    std::cerr << "Next index is      " << currPackIndex << std::endl << std::endl;
  }
  return currPackIndex-1;
}

void quitProgram(bool &stop){
    stop = true;
    std::cerr << std::endl;
    std::cerr << "Bye! | Tchau! | Tchüss!" << std::endl;
    std::cerr << std::endl;
}


// Internal functions

int waitResponse( const int &sckt, const unsigned int type ){
    package_t ack {};

    do{
      ssize_t val {recv(sckt, &ack, sizeof(ack), 0)};
      if ( val == -1 && ( errno == EAGAIN)){
        std::cout << "Timed out " << EAGAIN << std::endl;
        return -1;
      }

      if ( ack.type != type ){
        // TODO: Save somewhere
        continue;
      }

      //printPackage(ack);
      return ack.sequence;
    } while ( true );
}
