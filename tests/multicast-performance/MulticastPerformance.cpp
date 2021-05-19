/*
 *
 * Author: Clemens Bönnen (clemens.boennen@rwth-aachen.de)
 * Usage: Call with 'measure' on one device and 'respond' on several other devices and use 'multicast' or 'unicast'
 * Call: MulticastPerformance <measure|respond> <multicast|unicast>
 */

// Includes

#include <cstdio>
#include <cstring>

#include "rtps/rtps.h"

#include "McastReceiver.h"
#include "McastSender.h"
#include "McastOptions.h"


static bool responder = true;
static bool multicast = true;
static bool reliable = true;

void start();

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 4) {
        printf("[ ERROR ] Usage: MulticastPerformance <measure|respond> [<multicast|unicast>] [<reliable|best-effort>] Exit! \n");
        return -1;
    }

    if (strcmp(argv[1], "measure") == 0) {
        printf("[ INFO  ] Starting as measurment unit.\n");
        responder = false;
    } else if (strcmp(argv[1], "respond") == 0) {
        printf("[ INFO  ] Starting as responder unit.\n");
        responder = true;
    } else {
        printf("[ ERROR ] Use 'respond' or 'measure' as first argument. Exit! \n");
        return -1;
    }

    if (argc > 2 && strcmp(argv[2], "multicast") == 0) {
        printf("[ INFO  ] Using UDP Multicast for measurement.\n");
        multicast = true;
    } else if (argc > 2 && strcmp(argv[2], "unicast") == 0) {
        printf("[ INFO  ] Using UDP Unicast for measurement.\n");
        multicast = false;
    } else {
        printf("[ INFO  ] No argument set. Using multicast.\n");
        multicast = true;
    }

    if ((argc > 3 && strcmp(argv[3], "reliable") == 0) || (argc > 2 && strcmp(argv[2], "reliable") == 0)) {
        printf("[ INFO  ] Reader/Writer reliable.\n");
        reliable = true;
    } else if ((argc > 3 && strcmp(argv[3], "best-effort") == 0) || (argc > 2 && strcmp(argv[2], "reliable") == 0)) {
        printf("[ INFO  ] Reader/Writer best effort.\n");
        reliable = false;
    } else {
        printf("[ INFO  ] No arguemnt set. Using reliable Reader/Writer.\n");
        reliable = true;
    }

    rtps::init();

    printf("[ INFO  ] Running Tests!\n");
    start();
}

void start() {
    if (responder) {
        rtps::MulticastTests::MulticastReceiver recv(multicast, reliable);
        recv.run();
    } else {
        rtps::MulticastTests::MulticastSender send(multicast, reliable);
        send.run();
    }
}

