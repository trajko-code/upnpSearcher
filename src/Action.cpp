#include "setTopBox/Action.hpp"

#define XMLNS "http://schemas.xmlsoap.org/soap/envelope/"
#define ENCODING_STYLE "http://schemas.xmlsoap.org/soap/encoding/"

namespace setTopBox
{
    Action::Action(std::string name)
        :name(name)
    {
    }

    void Action::ShowAction() const
    {
        InOut::Out(this->name + '\n');
        InOut::Out("\t\tinput: ");
        for(auto const& arg : this->InputParameters)
        {
            arg.ShowArgument();
            InOut::Out(" ");
        }
        InOut::Out("\n\t\toutput: ");
        for(auto const& arg : this->OutputParameters)
        {
            arg.ShowArgument();
            InOut::Out(" ");
        }
        InOut::Out("\n");
    }

    void Action::AddArgument(Argument& argument)
    {
        if(argument.directionType == DirectionType::IN)
            this->InputParameters.push_back(argument);
        else
            this->OutputParameters.push_back(argument);
    }

    void Action::FillArgumentList(const std::string XMLAction, stateMap& stateTable)
    {
        std::string argumentList = XMLParser::GetTagValue(XMLAction, "argumentList");
        std::string argumentXML;

        std::string endArgumentTag = "</argument>";

        while( argumentList.length() > 0 )
        {
            argumentXML = XMLParser::GetTagValue(argumentList, "argument");
            this->ParseArgumentFromXML(argumentXML, stateTable);

            int cropPos = argumentList.find(endArgumentTag) + endArgumentTag.length();
            if(cropPos > argumentList.length())
                return;
            argumentList = argumentList.substr(cropPos);
        }
    }

    void Action::ParseArgumentFromXML(std::string argumentXML, stateMap& stateTable)
    {
        std::string name = XMLParser::GetTagValue(argumentXML, "name");
        DirectionType direction = XMLParser::GetTagValue(argumentXML, "direction")
            .compare("in") == 0 ? DirectionType::IN : DirectionType::OUT;
        std::string relatedStateVariable = XMLParser::GetTagValue(argumentXML, "relatedStateVariable");

        ArgumentDataFromTable argData = stateTable[relatedStateVariable];

        Argument newArgument(name, direction, relatedStateVariable, argData.type, argData.defaultValue, argData.sendEvents,
            argData.allowedValueList, Argument::AllowedValueRange{argData.min, argData.max, argData.step});

        this->AddArgument(newArgument);
    }

    bool Action::Execute(const std::string STBAddress, const std::string STBPort, 
        const std::string  serviceControlURL, const std::string serviceType)
    {
        try
        {
            std::string soapAction = serviceType + "#" + this->GetName();
            std::string argumentList = this->MakeArgumentForSOAPBody();
            std::string body = MakeSOAPRequestBody(serviceType, argumentList);

            std::string SOAPResponse = HTTPCommunicator::PostExecuteAction(serviceControlURL, STBAddress, STBPort, soapAction, body);
            if(SOAPResponse.empty())
                return false;
            else
            {
                ParseSOAPResponse(SOAPResponse);
                return true;
            }
            
        }
        catch(const std::string& ex)
        {
            InOut::Out(ex + '\n');
            return false;
        }   
    }

    std::string Action::MakeSOAPRequestBody(const std::string serviceType, const std::string argumentList)
    { 
        return  "<s:Envelope xmlns:s=\"" + std::string(XMLNS) + "\" "
                "s:encodingStyle=\"" + std::string(ENCODING_STYLE) + "\">\n"
                "<s:Body>\n"
                "<u:" + this->GetName() + " xmlns:u=\"" +  serviceType + "\">\n" 
                + argumentList +
                "</u:" + this->GetName() + ">"
                "</s:Body>\n"
                "</s:Envelope>\n";            
    }

    std::string Action::MakeArgumentForSOAPBody()
    {
        std::string arguments;
        std::string argumentName;
        std::string argumentValue;
        InOut::Out("Enter arguments for the action \"" + this->GetName() + "\" ('/' if the argument is not used):\n");
        for(auto const& arg : this->InputParameters)
        {
            argumentName = arg.GetName();
            InOut::Out("Argument: " + argumentName + "(" + arg.GetType() + ") " + arg.GetAdditionalInfo() + "Value: ");
            InOut::In(argumentValue);
            
            if(argumentValue.compare("/") != 0)
            {
                if(arg.CorrectType(argumentValue) && arg.CorrectValue(argumentValue))
                    arguments += "<" + argumentName +">" + argumentValue + "</" + argumentName + ">\n";
                else
                    throw std::string("ERROR: Invalid input argument!");
            }
        }
        return arguments;
    }

    void Action::ParseSOAPResponse(const std::string SOAPResponse)
    {
        std::string value;
        InOut::Out("### RESPONSE OK ###\n");
        for(auto const& outArg : this->OutputParameters)
        {
            value = XMLParser::GetTagValue(SOAPResponse, outArg.GetName());
            if(!value.empty())
                InOut::Out(outArg.GetName() + " : " + value + '\n');
        }
    }
}