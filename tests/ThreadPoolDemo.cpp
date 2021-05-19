/*
 *
 * Author: Andreas Wüstenberg (andreas.wuestenberg@rwth-aachen.de)
 */

#include "rtps/rtps.h"
#include "rtps/messages/MessageTypes.h"
#include "rtps/entities/Domain.h"

#include <iostream>


static const bool isPublisher = false;

void receiveCallback(void* /*callee*/, const rtps::ReaderCacheChange& cacheChange){
    std::array<uint8_t, 25> buffer;
    bool success = cacheChange.copyInto(buffer.data(), buffer.size());
    if(success){
        uint8_t offset = 4; // Encoding info and options
        printf("Received hello world message with index: %u\n", *reinterpret_cast<uint32_t*>(buffer.data() + offset));
    }else{
        printf("Received hello world message but copying failed\n");
    }
}


void startProgram(void*);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main() {
    rtps::init();

    sys_thread_new("Main Program", startProgram, nullptr, 1024, 3);
    while(true);
}

void startProgram(void* /*args*/){
    rtps::Domain domain;
    std::cout << "Size of domain: " << sizeof(domain) << "bytes.\n";

    auto part = domain.createParticipant();

    if(part == nullptr){
        printf("Failed to create participant");
        return;
    }

    char topicName[] = "HelloWorldTopic";
    char typeName[] = "HelloWorld";


    if(isPublisher) {
        rtps::Writer *writer = domain.createWriter(*part, topicName, typeName, false);
        if (writer == nullptr) {
            printf("Failed to create writer");
            return;
        }

        
    	domain.completeInit();

        char message[] = "Hello World";
        uint8_t buffer[20];
        ucdrBuffer microbuffer;
        uint32_t i = 0;
        while (true) {
            ucdr_init_buffer(&microbuffer, buffer, sizeof(buffer) / sizeof(buffer[0]));
            ucdr_serialize_array_uint8_t(&microbuffer, rtps::SMElement::SCHEME_CDR_LE.data(),
                                         rtps::SMElement::SCHEME_CDR_LE.size());
            ucdr_serialize_uint16_t(&microbuffer, 0); // options
            ucdr_serialize_uint32_t(&microbuffer, i++);
            ucdr_serialize_array_char(&microbuffer, message, sizeof(message) / sizeof(message[0]));
            auto change = writer->newChange(rtps::ChangeKind_t::ALIVE, buffer, sizeof(buffer) / sizeof(buffer[0]));
            if (change == nullptr) {
                printf("History full.\n");
                while (true);
            } else {
                //printf("Added change with id %i\n", i);
            }
            sys_msleep(200);

        }
    }else{
        rtps::Reader* reader = domain.createReader(*part, topicName, typeName, false);
        if(reader == nullptr){
            printf("Failed to create reader\n");
            return;
        }
        
    	domain.completeInit();

        reader->registerCallback(receiveCallback, nullptr);

        while(true){
            // Nothing to do. Just wait...
        }
    }
}

#undef MEASUREMENT
#undef RESPONDER
#undef PUB

#pragma clang diagnostic pop