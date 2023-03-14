#pragma once

#include <string>
#include <iostream>

struct TransferredMessage
{
    enum class TypeIndicator
    {
        ERROR,
        DATA
    };

    TypeIndicator Indicator;
    std::string Message;
};

inline std::ostream& operator<<(std::ostream& out, const TransferredMessage& msg)
{
    out << static_cast<int>(msg.Indicator) << ' ' << msg.Message;
    return out;
}
