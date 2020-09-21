#include <string>

class STB
{
    private:
        std::string uuid;
        std::string location;
        std::string nt;
    
    public:
        STB(std::string uuid, std::string location, std::string nt);
        
        std::string GetUUID() const { return this->uuid; }
};