# embeddedRTPS on Linux
This repository allows to compile and run embeddedRTPS on Linux for development purposes.
So far, this has been tested on Ubuntu 18.04.
The content and structure of this repository is work in progress. 

**[The main embeddedRTPS repository is located here.](https://github.com/embedded-software-laboratory/embeddedRTPS)**

## Dependencies

All dependencies are integrated as submodules to this repository. Make sure to use git clone --recursive when cloning.

## Compile

Run the following commands to compile the contents of this repository.  

```bash
git submodule update --init --recursive 
mkdir build
cd build
cmake ..
make -j4
```


## Run Unittests

After compiling, the set of unittests can be executed as follows:

```bash
cd build
./runTests
```
## HelloWorld Example with FastDDS

This repository also includes an example to demonstrate interoperability with FastDDS. For this to work, we first need to setup a tap interface that allows lwip to send network packets:
```bash
./setup-local-communication.sh
```

Start the embeddedRTPS part first:
```bash
build/fastddsComp [subscriber/publisher]
```

It is important to start the embeddedRTPS part first in order for lwip to initialize the tap interface, otherwise it will not be present when FastDDS searches for interfaces. Now, the FastDDS end can be launched:

```bash
build/HelloWorldExample [subscriber/publisher]
```

Note that with this setup, the communication only works when both embeddedRTPS and FastDDS are on the same machine. This has nothing to do with embeddedRTPS primarly, but with the tap inteface setup. We will provide a script that sets up the tap interface and a network bridge properly.

