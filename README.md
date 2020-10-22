# upnpSearcher
Discover and comunicate with STBs on local network using UPnP protocol.

## Features

*   Discover STBs on local network
*   Show discovered STBs
*   Select a specific STB
*   Show STB description
*   Pair to STB
*   Set STB friendly name
*   Send remote key command
*   Manually select service and action to send
*   Show services, actions and action arguments

## Platform

* Linux

## Dependency

[SocketLib](https://github.com/trajko-code/socketLib) is library for working with POSIX sockets.

## Build Requirements

*  [CMake](https://cmake.org/) minimum version 3.9

*  A C++11-standard-compliant compiler

## Build Application

**RECOMMENDED:** Before build application change friendly name and aplicationID in include/Config.hpp.

```cpp
namespace Config
{ 
    static std::string friendlyName = "upnpSearcher";
    static std::string aplicationID = "1234";
}
```

You can easily build application by running **run-build.sh** shell script with following command:
```shell
sh ./run-build.sh
```

## Example of use

After starting the application the main manu is displayed: 

![screenshot](screenshots/Menu/mainMenu.png)

After searching devices on local network (option 1), you can select one of descovered set-top box
(option 3) and STB menu will be displayed.

![screenshot](screenshots/Menu/STBMenu.png)


* **option 1** shows device description like uuid, friendly name, address, port, manufacturer...
* **option 2** sends request for pairing with device and ask you to enter PIN that will be show on TV screen
* **option 3** checks if device is paired
* **option 4** change device friendly name 
* **option 5** lists key codes and ask you to enter key code for remote controler
* **option 6** allows user to manually select service and service action that will be send to device 

## Class diagram

Following class diagram presents relations beetwen classes in this project.

![screenshot](screenshots/Diagrams/ClassDiagram/ClassDiagramRelations.png)

**Searcher** is the basic class that detects devices on the local network. It is implemented as singleton class.

![screenshot](screenshots/Diagrams/ClassDiagram/SearcherClass.png)

**HTTPCommunicator** and **XMLParser** are static classes that implement specific functions for HTTP comunication and parsing XML response.

![screenshot](screenshots/Diagrams/ClassDiagram/HTTPCommunicatorClass.png)      ![screenshot](screenshots/Diagrams/ClassDiagram/XMLParserClass.png)

Following class diagrams present classes inside **setTopBox** namespace that are abstract view of set-top box and its services and actions.

![screenshot](screenshots/Diagrams/ClassDiagram/STBClass.png)

![screenshot](screenshots/Diagrams/ClassDiagram/ServiceStruct.png)       ![screenshot](screenshots/Diagrams/ClassDiagram/ActionStruct.png)

![screenshot](screenshots/Diagrams/ClassDiagram/ArgumentStruct.png)

## Sequence diagram

Following sequence diagram shows order of action during pairing with new device.

![screenshot](screenshots/Diagrams/SequenceDiagram/SDPairToNewDevice.png)

Following sequence diagram shows order of action during sending remote key command to device that execute required action.

![screenshot](screenshots/Diagrams/SequenceDiagram/SDSendRemoteKeyCommand.png)

