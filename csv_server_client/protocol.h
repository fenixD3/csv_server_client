#pragma once

#include <string>
#include <thread>

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

struct Worker
{
public:
    std::thread Thread;

    template <typename TFunc>
    Worker(TFunc func)
        : Thread(std::move(func))
    {}

    ~Worker()
    {
        Thread.join();
    }
};
