#include "csv_server_client/protocol_serializer.h"

#include <gtest/gtest.h>
#include <iostream>

bool operator==(const TransferredMessage& lhs, const TransferredMessage& rhs)
{
    return std::tie(lhs.Indicator, lhs.Message) == std::tie(rhs.Indicator, rhs.Message);
}

TEST(MsgSerial, SerialAndDeserial)
{
    TransferredMessage expected{TransferredMessage::TypeIndicator::DATA, "Hello"};

    auto serial_data = ProtocolSerializer::Serialize(expected);
    std::clog << serial_data << std::endl << "Size: " << serial_data.size() << std::endl;

    TransferredMessage after_way = ProtocolSerializer::Deserialize(std::move(serial_data));
    EXPECT_EQ(expected, after_way);
}
