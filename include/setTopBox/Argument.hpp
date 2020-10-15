#include "setTopBox/STBTypes.hpp"
#include "InOut.hpp"
#include <algorithm> 

namespace setTopBox
{
    struct Argument
    {
        private:
            friend class Action;

            std::string name;
            DirectionType directionType;
            std::string relatedStateVariable;
            std::string type;
            std::string defaultValue;
            bool sendEvents;
            std::vector<std::string> allowedValueList;
            struct AllowedValueRange
            {
                std::string minimum;
                std::string maximum;
                std::string step;
            } valueRange;

            Argument(std::string name, DirectionType directionType, std::string relatedStateVariable,
                std::string type, std::string defaultValue, bool sendEvents, 
                std::vector<std::string> allowedList, struct AllowedValueRange valueRange);
            
            void ShowArgument() const;
            bool CorrectType(const std::string argumentValue) const;
            bool CorrectValue(const std::string argumentValue) const;

            std::string GetAdditionalInfo() const;
            std::string GetName() const { return this->name; }
            std::string GetType() const { return this->type; }
            std::string GetDefaultValue() const { return this->defaultValue; }
    };
}