
#include "functions-server.hpp"


void receivePackage(const int &sckt, package_t &pckg){
    ssize_t val {recv(sckt, &pckg, sizeof(pckg), 0)};

    if ( val == -1 && ( errno == EAGAIN ) ){
      return;
    }

    if ( val >= 0 && pckg.type == START_PACKAGE){
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
        package_t * packs {receiveOtherPacks(sckt, pckg.seq+1)};
        handlePacks(packs);
    }
    else {
        std::cerr << "Error receiving message" << std::endl;
        printPackage(pckg);
    }
}


package_t * receiveOtherPacks(const int &sckt, unsigned int seq){
  package_t * packs {(package_t *)malloc(64 * sizeof(package_t))};
  unsigned int lastRecv {0};
  unsigned int pckgArraySize {64};

  package_t pckg {0};
  const unsigned int windowSize {5};

  srand(time(NULL));

  for (;;){
    std::array<package_t, windowSize> receiving {0};
    std::array<bool, windowSize> oks {};
    unsigned int errIndex = windowSize;
    for ( unsigned int i {0}; i < windowSize; i++ ) oks[i] = false;
    for ( unsigned int i {0}; i < windowSize; i++){
      ssize_t val {recv(sckt, &pckg, sizeof(pckg), 0)};
      //If timeout
      if ( val == -1 && ( errno == EAGAIN ) ){ break; }

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

      //Check corruption of package (add commented rand to test)
      if ( pckg.header != PACKAGE_START_MARK || !validateCRC(pckg) /*|| (rand() % 100 < 10)*/) {
        errIndex = std::min(errIndex, (unsigned int)difference);
        oks[difference] = false;
        continue;
      }

      //Save otherwise
      std::cerr << "Got new package" << std::endl;
      printPackage(pckg);
      receiving[difference] = pckg;
      oks[difference] = true;
      if (pckg.type == END_PACKAGE) break;
    }
    std::cerr << "errIndex: " << errIndex << std::endl;
    for ( unsigned int i {0}; i < windowSize; i++ ){
      std::cerr << oks[i] << " ";
    }
    std::cerr << std::endl;

    // Copy good receives to permanent
    std::cerr << "Copying good receives" << std::endl;
    for ( unsigned int i {0}; i < windowSize && i < errIndex && oks[i]; i++ ){
      packs[lastRecv++] = receiving[i];
      seq = (seq + 1) % 16;
      if (lastRecv >= pckgArraySize) {
        pckgArraySize += 64;
        packs = (package_t*)realloc(packs, pckgArraySize * sizeof(package_t));
      }
    }
    package_t response {0};
    // If no package got corrupted respond with ack else respond with nack
    if ( errIndex == windowSize ){
        initPackage(response, ACK_PACKAGE);
        response.seq = (seq - 1) % 16;
    } else {
        initPackage(response, NACK_PACKAGE);
        response.seq = seq;
    }
    std::cerr << "Sending response for sliding window: " << std::endl;
    printPackage(response);
    std::cerr << std::endl << std::endl;
    ssize_t ret {send(sckt, &response, sizeof(package_t), 0)}; 
    if ( ret < 0 ){
        std::cerr << "Error sending response for sliding window" << std::endl;
        std::cerr << std::endl;
        std::cerr << errno << std::endl;
        return NULL;
    }
    if ( lastRecv > 0 && packs[lastRecv-1].type == END_PACKAGE ) {
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
      handleMessages((char*)data);
      break;
    case MEDIA_PACKAGE:
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


void handleMedias(){
    // File message's format
    // [<data e hour>]<User> : Arquivo de midia enviado! O arquivo pode ser encontrado em <caminho_para_arquivo>

    printDate();
    std::cout << "<Usuário> : " 
        << "Arquivo de midia enviado! O arquivo pode ser encontrado em " 
        << "<caminho_para_arquivo>" 
        << std::endl << std::endl;
}
