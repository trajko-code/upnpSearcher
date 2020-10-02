#include "Searcher.h"

#define MX_DELAY "2"
#define SEARCH_TIME 2

int getInt(std::string input);

int main()
{
    Searcher cp("friendlyName");
    std::string entry;
    std::string ordinalNumber;
    uint number;
    std::string serviceEntry;
    std::string serviceNumberEntry;
    uint serviceNumber;
    std::string actionEntry;
    std::string actionNumberEntry;
    uint actionNumber;
    bool back;

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
            cp.ClearDetectedSTBs();
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
            std::cout << "STB services: \n";
            stb->ShowMyServices();
            while(!back)
            {
                std::cout << '\n' << "**STB menu**\n";
                std::cout << "(0) Back\n";
                std::cout << "(1) Select service\n";
                std::cout << "Enter number: \n";
                std::cout << ">";

                std::cin >> serviceEntry;
                if(serviceEntry.compare("1") == 0)
                {
                    
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
                                //actionNumber = getInt(actionNumberEntry);

                                //zahtev za unosom argumenata
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
                else if(serviceEntry.compare("0") == 0)
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