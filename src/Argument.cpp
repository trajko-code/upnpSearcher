#include "setTopBox/Argument.hpp"

namespace setTopBox
{
    Argument::Argument(std::string name, DirectionType directionType, 
                            std::string relatedStateVariable, std::string type, 
                            std::string defaultValue, bool sendEvents,
                            std::vector<std::string> allowedList,
                            struct AllowedValueRange valueRange)
        :name(name), directionType(directionType), relatedStateVariable(relatedStateVariable), 
        type(type), defaultValue(defaultValue), sendEvents(sendEvents), 
        allowedValueList(allowedList), valueRange(valueRange)
    {
    }

    void Argument::ShowArgument() const
    {
        InOut::Out("{" + this->name + " " + this->GetType() + " events:" + (this->sendEvents?"yes":"no") + "}");
    }

    std::string Argument::GetAdditionalInfo() const
    {
        std::string info;
        if(!this->GetDefaultValue().empty())
            info += "Default value: " + this->GetDefaultValue() + " ";
        if(this->allowedValueList.size() > 0)
        {
            info += "Allowed values ( ";
            for(auto const& value : this->allowedValueList)
                info += value + " ";
            info += ")";
        }
        if(!this->valueRange.minimum.empty())
            info += "Value range: minimum - " + this->valueRange.minimum + 
                " maximum - " + this->valueRange.maximum + "step - " + this->valueRange.step;
        info += "\n";
        return info;
    }

    bool Argument::CorrectType(std::string inputValue) const
    {
        if(!this->GetType().compare("string"))
                return true;
        else if(!this->GetType().compare("int") || !this->GetType().compare("i4")) 
        {
            try
            {
                std::stoi(inputValue);
                return true;
            }
            catch(...)
            {
                return false;
            }
        }
        else if(!this->GetType().compare("float"))
        {
            try
            {
                std::stof(inputValue);
                return true;
            }
            catch(...)
            {
                return false;
            }
        }
        else if(!this->GetType().compare("char"))
            return inputValue.length() == 1;
        else if(!this->GetType().compare("boolean"))
            return std::find_if( booleanValues.begin(), booleanValues.end(),
                [inputValue](const std::string& s){return !inputValue.compare(s); }) != booleanValues.end();
        else
            return false;
    }

    bool Argument::CorrectValue(std::string inputValue) const
    {
        if(this->allowedValueList.size() > 0)
        {
            auto allowedValue = std::find_if(this->allowedValueList.begin(), this->allowedValueList.end(), 
                [&inputValue](const std::string& value) { return !value.compare(inputValue); });

            if(allowedValue == this->allowedValueList.end())
                return false;
        }
        return true;
    }
}