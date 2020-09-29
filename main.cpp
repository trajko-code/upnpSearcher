#include "Searcher.h"

int main()
{
    Searcher cp("friendlyName");

    if(cp.SearchBcast("2", 3) > 0)
    {
       cp.ShowDetectedSTBs();
    }
    else
    {
        std::cout<<"No STB detected."<<std::endl;
    }

    return 0;
}