#include "protocol_serializer.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <sstream>

std::string ProtocolSerializer::Serialize(const TransferredMessage& msg)
{
    std::ostringstream output;
    boost::archive::text_oarchive(output) << msg;

    std::string result = std::move(output).str();
    result += EndSuffix;
    return result;
}

TransferredMessage ProtocolSerializer::Deserialize(std::string&& msg)
{
    msg.erase(std::next(msg.crbegin(), EndSuffix.size()).base(), msg.crbegin().base());

    std::istringstream input(std::move(msg));
    TransferredMessage result;
    boost::archive::text_iarchive(input) >> result;
    return result;
}
