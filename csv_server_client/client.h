#pragma once

#include "protocol.h"

#include <string>

#include <boost/asio/ip/tcp.hpp>

namespace client
{

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace sys = boost::system;

class Client
{
private:
    tcp::socket m_Socket;
    tcp::endpoint m_Endpoint;
    std::string m_TransferringText;

public:
    Client(asio::io_context& ioc, const tcp::endpoint& endpoint, std::string&& text);

    void RunAndConnect();

private:
    void WriteRequest();
    void ReadResponse();
    void ProcessResponse(const TransferredMessage& response);
};

}

