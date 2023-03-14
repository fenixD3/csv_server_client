#include "client.h"
#include "server.h"
#include "request_handler.h"

#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <optional>
#include <sstream>

#include <boost/asio/signal_set.hpp>

namespace
{

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace sys = boost::system;

void PrintUsage()
{
    std::cout << "[server | client] [options]\n"
                 "server: noting\n"
                 "client: <path to file>" << std::endl;
}

const auto DefaultIp = asio::ip::make_address("127.0.0.1");
constexpr asio::ip::port_type DefaultPort = 3333;

template <typename TWork>
void RunWorkers(unsigned int thread_nums, const TWork& func)
{
    std::vector<std::jthread> workers;
    workers.reserve(thread_nums - 1);
    while (--thread_nums)
    {
        workers.emplace_back(func);
    }
    func();
}

void StartClient(const std::string& file_path)
{
    auto read_file = [&file_path]() -> std::optional<std::string>
    {
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            std::cerr << "Fail to read file" << std::endl;
            return std::nullopt;
        }

        std::ostringstream data;
        data << file.rdbuf();

        return std::move(data).str();
    };

    auto csv_text = read_file();
    if (!csv_text)
    {
        std::cerr << "CSV wasn't read" << std::endl;
        return;
    }

    asio::io_context ioc;
    client::Client(ioc, {DefaultIp, DefaultPort}, std::move(*csv_text)).RunAndConnect();
}

void StartServer()
{
    const unsigned int thread_nums = std::max(1u, std::thread::hardware_concurrency() / 4);

    asio::io_context ioc(thread_nums);
    asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number)
        {
            if (!ec)
            {
                ioc.stop();
            }
        });

    auto handler = std::make_shared<server::RequestHandler>();
    server::LaunchServer(
        ioc, {DefaultIp, DefaultPort},
        [handler](std::string&& text, auto&& send_func)
        {
            (*handler)(std::move(text), std::forward<decltype(send_func)>(send_func));
        });

    RunWorkers(thread_nums, [&ioc] { ioc.run(); });
    std::cout << "server is stopping" << std::endl;
}

}

int main(int ac, char **av)
{
    const std::string ServerParameter = "server";
    const std::string ClientParameter = "client";

    if (ac < 2
        || (av[1] != ServerParameter && av[1] != ClientParameter)
        || (av[1] == ClientParameter && ac < 3))
    {
        PrintUsage();
        return EXIT_FAILURE;
    }

    if (av[1] == ClientParameter)
    {
        StartClient(av[2]);
    }
    if (av[1] == ServerParameter)
    {
        StartServer();
    }
}