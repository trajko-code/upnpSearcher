#include "Searcher.hpp"
#include "Config.hpp"
#include "InOut.hpp"
#include "setTopBox/KeyCodes.hpp"

#define MX_DELAY "2"
#define SEARCH_TIME 1

int getInt(std::string input);
void printMainMenu();
void printSTBMenu();
void printServiceMenu();

int main()
{
    auto& cp = Searcher::GetInstance();
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
        printMainMenu();

        if(!InOut::In(entry))
        {
            InOut::Out("FATAL ERROR: cin error!\n");
            return -1;
        }   
        
        if(entry.compare("1") == 0)
        {
            InOut::Out("Searching devices...\n");
            cp.ClearDetectedSTBs();
            if(cp.SearchBcast(MX_DELAY, SEARCH_TIME) > 0)
            {
                InOut::Out(">>>DETECTED DEVICES<<<\n");
                cp.ShowDetectedSTBs();
            }
            else
                InOut::Out("No STB detected.\n");
        }
        else if(entry.compare("2") == 0)
        {
            if(cp.GetDetectedSTBsCount() > 0)
                cp.ShowDetectedSTBs();
            else
                InOut::Out("No STB detected.\n");
        }
        else if(entry.compare("3") == 0)
        {
            std::shared_ptr<setTopBox::STB> stb;
            InOut::Out("Enter the ordinal number of the device:\n");
            InOut::In(ordinalNumber);
            number = getInt(ordinalNumber);
            if(number > 0 && number <= cp.GetDetectedSTBsCount())
                stb = cp.GetSTB(number - 1);
            else
            {
                InOut::Out("!!! Wrong entry !!!\n");
                continue;
            }

            back = false;
            InOut::Out("Selected STB: " + stb->GetFriendlyName() + '\n');
            while(!back)
            {
                printSTBMenu();

                InOut::In(stbEntry);
                if(stbEntry.compare("1") == 0)
                {
                    stb->ShowDescription();
                }
                else if(stbEntry.compare("2") == 0)
                {
                    if(stb->SendPairingRequest())
                    {
                        std::string pin;
                        InOut::Out("Enter PIN: ");
                        InOut::In(pin);
                        stb->SendPairingCheck(pin);
                    }
                }
                else if(stbEntry.compare("3") == 0)
                {
                    if(stb->CheckIsPaired())
                        InOut::Out("Device is paired.\n");
                    else
                        InOut::Out("Device is not paired.\n");
                }
                else if(stbEntry.compare("4") == 0)
                {
                    InOut::Out("Enter new friendly name: \n");
                    InOut::GetLine(friendlyName);
                    stb->SetDeviceFriendlyName(friendlyName);
                }
                else if(stbEntry.compare("5") == 0)
                {
                    stb->ShowKeysName();
                    backRemoteControler = true;
                    while(backRemoteControler)
                    {
                        InOut::Out("Enter key number (or 0 to exit): \n");
                        InOut::In(keyNumber);
                        keyCode = getInt(keyNumber);
                        if(keyCode > 0 && keyCode <= setTopBox::keyCodes.size())
                        {
                            if(!stb->SendKeyCommand(keyCode-1))
                                backRemoteControler = false;
                        }
                        else if(keyCode == 0)
                            backRemoteControler = false;
                        else
                            InOut::Out("!!! Wrong entry !!!\n");
                    }
                }
                else if(stbEntry.compare("6") == 0)
                {
                    InOut::Out("Available STB services: \n");
                    stb->ShowMyServices();
                    InOut::Out("Enter the ordinal number of the service:\n");
                    InOut::In(serviceNumberEntry);
                    serviceNumber = getInt(serviceNumberEntry);
                    if(serviceNumber > 0 && serviceNumber <= stb->GetDetectedServicesCount())
                    {
                        InOut::Out("Selected service: " + stb->GetServiceName(serviceNumber - 1) + '\n');
                        InOut::Out("Service actions: \n");
                        stb->ShowServiceActions(serviceNumber - 1);

                        bool endServiceMenu = false;
                        while(!endServiceMenu)
                        {
                            printServiceMenu();
                            
                            InOut::In(actionEntry);
                            if(actionEntry.compare("1") == 0)
                            {
                                
                                InOut::Out("Enter the ordinal number of the action:\n");
                                InOut::In(actionNumberEntry);
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
                        InOut::Out("!!! Wrong entry !!!\n");
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
            InOut::Out("!!! Wrong entry !!!\n");
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

void printMainMenu()
{
    std::string mainMenu = "\n*Main menu*\n"
                        "(0) Quit\n"
                        "(1) Search STBs\n"
                        "(2) Show detected STBs\n"
                        "(3) Select STB\n"
                        "Enter number:\n"
                        ">";
    InOut::Out(mainMenu);
}

void printSTBMenu()
{
    std::string STBMenu = "\n**STB menu**\n"
                        "(0) Back\n"
                        "(1) Device description\n"
                        "(2) Pair to device\n"
                        "(3) Is paired\n"
                        "(4) Set friendly name\n"
                        "(5) Remote Controler\n"
                        "(6) Select service (advanced)\n"
                        "Enter number: \n"
                        ">";
    InOut::Out(STBMenu);
}

void printServiceMenu()
{
    std::string serviceMenu = "\n***Service menu***\n"
                            "(0) Back\n"
                            "(1) Execute action\n"
                            "Enter number:\n"
                            ">";
    InOut::Out(serviceMenu);
}