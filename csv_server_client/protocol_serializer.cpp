#include "protocol_serializer.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <sstream>

std::string ProtocolSerializer::Serialize(const TransferredMessage& msg)
{
    std::ostringstream output;
    boost::archive::text_oarchive(output) << msg;
    return output.str();
}

TransferredMessage ProtocolSerializer::Deserialize(std::string&& msg)
{
    std::istringstream input(std::move(msg));
    TransferredMessage result;
    boost::archive::text_iarchive(input) >> result;
    return result;
}
