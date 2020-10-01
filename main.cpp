#include "Searcher.h"

#define MX_DELAY "2"
#define SEARCH_TIME 3

int main()
{
    int searchTime = SEARCH_TIME;
    Searcher cp("friendlyName");
    
    bool back;

    while(true)
    {
        std::cout << std::endl << "*Main menu*" << std::endl;
        std::cout << "0. Quit" << std::endl;
        std::cout << "1. Search STBs" << std::endl;
        std::cout << "2. Select STB" << std::endl;
        std::cout << "Enter number:" << std::endl;
        std::cout << ">";

        uint entry;
        scanf("%u", &entry);
        if(entry == 1)
        {
            std::cout << "Searching devices..." << std::endl;
            cp.ClearDetectedSTBs();
            if(cp.SearchBcast(MX_DELAY, searchTime) > 0)
            {
                std::cout<<"///DETECTED DEVICES///"<<std::endl;
                cp.ShowDetectedSTBs();
            }
            else
                std::cout<<"No STB detected."<<std::endl;
        }
        else if(entry == 2)
        {
            std::shared_ptr<STB> stb;
            std::cout << "Enter the ordinal number of the device:" << std::endl;
            uint ordinalNumber;
            scanf("%u", &ordinalNumber);
            if(ordinalNumber <= cp.GetDetectedSTBsCount())
                stb = cp.GetSTB(ordinalNumber - 1);
            else
            {
                std::cout << "Wrong entry" << std::endl;
                continue;
            }

            back = false;
            std::cout << "Selected STB: " << stb->GetFriendlyName() << std::endl;
            std::cout << "STB services: " << std::endl;
            stb->ShowMyServices();
            while(!back)
            {
                std::cout << std::endl << "**STB menu**" << std::endl;
                std::cout << "(0) Back" << std::endl;
                std::cout << "(1) Select service" << std::endl;
                std::cout << "Enter number: " << std::endl;
                std::cout << ">";

                uint serviceEntry;
                scanf("%u", &serviceEntry);
                if(serviceEntry == 1)
                {
                    uint serviceNumber;
                    std::cout << "Enter the ordinal number of the service:" << std::endl;
                    scanf("%u", &serviceNumber);
                    if(serviceNumber <= stb->GetDetectedServicesCount())
                    {
                        std::cout << "Selected service: " << stb->GetServiceName(serviceNumber - 1) << std::endl;
                        std::cout << "Service actions: " << std::endl;
                        stb->ShowServiceActions(serviceNumber - 1);

                        bool endServiceMenu = false;
                        while(!endServiceMenu)
                        {
                            std::cout << std::endl << "***Service Menu***" << std::endl;
                            std::cout << "(0) Back" << std::endl;
                            std::cout << "(1) Execute action" << std::endl;
                            std::cout << "Enter number: " << std::endl;
                            
                            uint actionEntry;
                            scanf("%u", &actionEntry);
                            if(actionEntry == 1)
                            {
                                uint actionNumber;
                                std::cout << "Enter the ordinal number of the action:" << std::endl;
                                scanf("%u", &actionNumber);

                                //zahte za unosom argumenata
                            }
                            else if(actionEntry == 0)
                                endServiceMenu = true;

                        }
                    }
                    else
                    {
                        std::cout << "Wrong entry" << std::endl;
                        continue;
                    }
                }
                else if(serviceEntry == 0)
                    back = true;
            }
        }
        else if(entry == 0)
            break;
    }
    return 0;
}