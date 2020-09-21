#include "Searcher.h"

int main()
{
    Searcher cp("ContPt", "123");

    cp.SearchBcast(std::to_string(2), 10);

    return 0;
}