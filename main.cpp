#include "Searcher.h"

int main()
{
    Searcher cp("friendlyName");

    if(cp.SearchBcast(std::to_string(4), ALIVE, 15) > 0)
    {
        std::string uuid;
        std::cout<<"Insert uuid of device: "<<std::endl;
        std::cin.clear();
        std::cin >> uuid;

        cp.SearchSTBDescription(uuid);
    }
    else
    {
        std::cout<<"No STB detected."<<std::endl;
    }

    return 0;
}