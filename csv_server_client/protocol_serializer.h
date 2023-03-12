#pragma once

#include "protocol.h"

template <typename TArchive>
void serialize(TArchive& ar, TransferredMessage& object, [[maybe_unused]] const unsigned int version)
{
    ar & object.Indicator;
    ar & object.Message;
}

class ProtocolSerializer
{
public:
    static std::string Serialize(const TransferredMessage& msg);
    static TransferredMessage Deserialize(std::string&& msg);
};
