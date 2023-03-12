#include "server.h"

#include <iostream>
#include <thread>
#include <vector>

//#include <boost/asio/ip/tcp.hpp>

#include "protocol.h"

template <typename TWork>
void RunWorkers(unsigned int thread_nums, TWork func)
{
    std::vector<Worker> workers;
    while (--thread_nums)
    {
        workers.emplace_back(func);
    }
    func();
}

namespace
{

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

void PrintUsage()
{
    std::cout << "[server | client] [options]\n"
                 "server: noting\n"
                 "client: <path to file>" << std::endl;
}

const auto DefaultIp = asio::ip::make_address("127.0.0.1");
constexpr unsigned short DefaultPort = 3333;

void StartClient()
{
    boost::asio::io_context io_context;
}

void StartServer()
{
    asio::io_context ioc;
    const unsigned int thread_nums = std::thread::hardware_concurrency() / 4;

    RunWorkers(thread_nums, [&ioc] { ioc.run(); });
}

}

int main(int ac, char **av)
{
    const std::string ServerParameter = "server";
    const std::string ClientParameter = "client";

    if (ac < 2
        || (av[1] != ServerParameter || av[1] != ClientParameter)
        || (av[1] == ClientParameter && ac < 3))
    {
        PrintUsage();
        return EXIT_FAILURE;
    }

    if (av[1] == ClientParameter)
    {
        StartClient();
    }
    if (av[1] == ServerParameter)
    {
        StartServer();
    }
}