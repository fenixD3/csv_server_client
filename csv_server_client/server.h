#pragma once

#include <memory>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

namespace server
{

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

template <typename THandler>
class Listener : public std::enable_shared_from_this<Listener<THandler>>
{
private:
    asio::io_context& m_Ioc;
    tcp::acceptor m_Acceptor;
    THandler m_Handler;

public:
    template <typename UHandler>
    Listener(asio::io_context& ioc, const tcp::endpoint& endpoint, UHandler&& handler)
        : m_Ioc(ioc)
        , m_Acceptor(asio::make_strand(ioc))
        , m_Handler(std::forward<UHandler>(handler))
    {
        m_Acceptor.open(endpoint.protocol());
        m_Acceptor.set_option(asio::socket_base::reuse_address(true));
        m_Acceptor.bind(endpoint);
        m_Acceptor.listen(asio::socket_base::max_listen_connections);
    }

    void Run()
    {
        DoAccept();
    }

private:
    void DoAccept()
    {

    }
};

template <typename THandler>
void LaunchServer(asio::io_context& ioc, const tcp::endpoint& endpoint, THandler&& handler)
{
    std::make_shared<Listener>(ioc, endpoint, std::forward<THandler>(handler))->Run();
}

}
