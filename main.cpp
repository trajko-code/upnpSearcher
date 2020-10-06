#include "Searcher.h"
#include "Configuration.h"

#define MX_DELAY "2"
#define SEARCH_TIME 2

int getInt(std::string input);

int main()
{
    Searcher cp(Config::friendlyName);
    std::string entry;
    std::string ordinalNumber;
    uint number;
    std::string stbEntry;
    std::string serviceNumberEntry;
    uint serviceNumber;
    std::string actionEntry;
    std::string actionNumberEntry;
    uint actionNumber;
    std::string friendlyName;
    std::string keyNumber;
    int keyCode;
    bool back;
    bool backRemoteControler;

    while(true)
    {
        std::cout << "\n*Main menu*\n";
        std::cout << "(0) Quit" << '\n';
        std::cout << "(1) Search STBs\n";
        std::cout << "(2) Select STB\n";
        std::cout << "Enter number:\n";
        std::cout << ">";

        std::cin >> entry;
        if(entry.compare("1") == 0)
        {
            std::cout << "Searching devices...\n";
            //cp.ClearDetectedSTBs();
            if(cp.SearchBcast(MX_DELAY, SEARCH_TIME) > 0)
            {
                std::cout << ">>>DETECTED DEVICES<<<\n";
                cp.ShowDetectedSTBs();
            }
            else
                std::cout<<"No STB detected.\n";
        }
        else if(entry.compare("2") == 0)
        {
            std::shared_ptr<STB> stb;
            std::cout << "Enter the ordinal number of the device:\n";
            std::cin >> ordinalNumber;
            number = getInt(ordinalNumber);
            if(number > 0 && number <= cp.GetDetectedSTBsCount())
                stb = cp.GetSTB(number - 1);
            else
            {
                std::cout << "!!! Wrong entry !!!\n";
                continue;
            }

            back = false;
            std::cout << "Selected STB: " << stb->GetFriendlyName() << '\n';
            while(!back)
            {
                std::cout << '\n' << "**STB menu**\n";
                std::cout << "(0) Back\n";
                std::cout << "(1) Device description\n";
                std::cout << "(2) Pair to device\n";
                std::cout << "(3) Is paired\n";
                std::cout << "(4) Set friendly name\n";
                std::cout << "(5) Remote Controler\n";
                std::cout << "(6) Select service (advanced)\n";
                std::cout << "Enter number: \n";
                std::cout << ">";

                std::cin >> stbEntry;
                if(stbEntry.compare("1") == 0)
                {
                    stb->ShowDescription();
                }
                else if(stbEntry.compare("2") == 0)
                {
                    stb->PairToDevice(); // bool
                }
                else if(stbEntry.compare("3") == 0)
                {
                    if(stb->CheckIsPaired())
                        std::cout << "Device is paired.\r\n";
                    else
                        std::cout << "Device is not paired.\r\n";
                    
                }
                else if(stbEntry.compare("4") == 0)
                {
                    std::cout << "Enter new friendly name: \n";
                    std::cin >> friendlyName;
                    stb->SetDeviceFriendlyName(friendlyName);
                }
                else if(stbEntry.compare("5") == 0)
                {
                    stb->ShowKeysName();
                    backRemoteControler = true;
                    while(backRemoteControler)
                    {
                        std::cout << "Enter key number (or 0 to exit): \n";
                        std::cin >> keyNumber;
                        keyCode = getInt(keyNumber);
                        if(keyCode > 0 && keyCode <= Config::keys.size())
                            stb->SendKeyCommand(keyCode-1);
                        else if(keyCode == 0)
                            backRemoteControler = false;
                        else
                            std::cout << "!!! Wrong entry !!!\n";
                    }
                }
                else if(stbEntry.compare("6") == 0)
                {
                    std::cout << "Available STB services: \n";
                    stb->ShowMyServices();
                    std::cout << "Enter the ordinal number of the service:\n";
                    std::cin >> serviceNumberEntry;
                    serviceNumber = getInt(serviceNumberEntry);
                    if(serviceNumber > 0 && serviceNumber <= stb->GetDetectedServicesCount())
                    {
                        std::cout << "Selected service: " << stb->GetServiceName(serviceNumber - 1) << '\n';
                        std::cout << "Service actions: \n";
                        stb->ShowServiceActions(serviceNumber - 1);

                        bool endServiceMenu = false;
                        while(!endServiceMenu)
                        {
                            std::cout << '\n' << "***Service menu***\n";
                            std::cout << "(0) Back" << '\n';
                            std::cout << "(1) Execute action\n";
                            std::cout << "Enter number:\n";
                            
                            std::cin >> actionEntry;
                            if(actionEntry.compare("1") == 0)
                            {
                                
                                std::cout << "Enter the ordinal number of the action:\n";
                                std::cin >> actionNumberEntry;
                                actionNumber = getInt(actionNumberEntry);
                                if(actionNumber > 0 && actionNumber <= stb->GetServiceActionsCount(serviceNumber-1))
                                    stb->ExecuteServiceAction(serviceNumber-1, actionNumber-1);
                            }
                            else if(actionEntry.compare("0") == 0)
                                endServiceMenu = true;
                        }
                    }
                    else
                    {
                        std::cout << "!!! Wrong entry !!!\n";
                        continue;
                    }
                }
                else if(stbEntry.compare("0") == 0)
                    back = true;
            }
        }
        else if(entry.compare("0") == 0)
            break;
        else
            std::cout << "!!! Wrong entry !!!\n";
    }

    return 0;
}

int getInt(std::string input)
{
    try
    {
        return std::stoi(input);
    }
    catch(...)
    {
        return -1;
    }
    
}