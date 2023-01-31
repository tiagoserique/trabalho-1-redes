#include "functions-client.hpp"

// --------------------------
// Internal functions headers
// --------------------------

unsigned int waitAck(unsigned int seq, const int &sckt);


// ---------------
// Implementations
// ---------------

void sendMessage(const int &sckt){
    char ch {};
    package_t start {};
    std::string message {};
    
    while ( (ch = (char) std::cin.get()) != STOP_INSERT_COMMAND ) message += ch;

    initPackage(start, START_PACKAGE);
    ssize_t ret {send(sckt, &start, sizeof(package_t), 0)}; 
    if ( ret < 0 ){
        std::cerr << "Error sending message" << std::endl;
        std::cerr << std::endl;
        std::cerr << errno << std::endl;
        return;
    }
    package_t * packs {divideData(
                      (void *)message.c_str(),
                      message.size(),
                      TEXT_PACKAGE,
                      0)};

    unsigned int packI {0};
    package_t currPack = packs[0];
    while ( currPack.type != END_PACKAGE ){
        // Send
        currPack = packs[packI];
        ret = send(sckt, &currPack, sizeof(package_t), 0); 
        if ( ret < 0 ){
            std::cerr << "Error sending message" << std::endl;
            std::cerr << std::endl;
            std::cerr << errno << std::endl;
            return;
        }

        // Wait for ack ( if timed out resend )
        packI += waitAck( (unsigned int)currPack.sequence, sckt);
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


void quitProgram(bool &stop){
    stop = true;
    std::cerr << std::endl;
    std::cerr << "Bye! | Tchau! | Tchüss!" << std::endl;
    std::cerr << std::endl;
}


// Internal functions

/*
    @brief Waits for ack signal

    @param seq (unsigned int) : desired sequence number of ack package
    @param sckt (const int &) : The File Descriptor of the socket

    @return 1 if received ack
    @return 0 if timed out
*/
unsigned int waitAck(unsigned int seq, const int &sckt){
    package_t ack {};

    do{
      ssize_t val {recv(sckt, &ack, sizeof(ack), 0)};
      if ( val == -1 && ( errno = EAGAIN)){
        std::cout << "Timed out" << std::endl;
        return 0;
      }

      if ( ack.type != ACK_PACKAGE ){
        // TODO: Save somewhere
        continue;
      }

      if ( (unsigned int)ack.sequence != seq ){
        // TODO: Save somewhere
        continue;
      }

      printPackage(ack);
      return 1;
    } while ( true );
}
