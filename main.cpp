#include "Searcher.h"

int main()
{
    Searcher cp("friendlyName");

    if(cp.SearchBcast(std::to_string(4), ALIVE, 10) > 0)
    {
       cp.ShowDetectedSTBs();
    }
    else
    {
        std::cout<<"No STB detected."<<std::endl;
    }

    return 0;
}