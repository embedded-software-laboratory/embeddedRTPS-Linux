// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file HelloWorld_main.cpp
 *
 */

#include "McastResponder.h"

#include <fastrtps/Domain.h>
#include <fastrtps/log/Log.h>

using namespace eprosima;
using namespace fastrtps;
using namespace rtps;
int main(int argc, char** argv)
{
    Log::SetVerbosity(Log::Kind::Info);
    std::cout << "[ INFO  ] Starting "<< std::endl;
    bool multicast = false;
    bool reliable = true;
    if(argc == 2)
    {
        if(strcmp(argv[1],"multicast")==0)
        {
            multicast = true;;
        }
        else if(strcmp(argv[1],"unicast")==0)
            multicast = false;

        if(strcmp(argv[2],"reliable")==0)
        {
            reliable = true;;
        }
        else if(strcmp(argv[2],"best-effort")==0)
            reliable = false;

    }
    else
    {
        std::cout << "[ ERROR ] Argument failure! Use >>multicast<< or >>unicast<< as first argument and >>reliable<< or >>best-effort<< as second argument." << std::endl;
        Log::Reset();
        return 0;
    }
    McastResponder responder(multicast, reliable);
    if(responder.init())
    {
        std::cout << "[ INFO  ] Running..." << std::endl;
        responder.run();
    }
    Domain::stopAll();
    Log::Reset();
    return 0;
}
