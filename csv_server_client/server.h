#pragma once

#include "protocol_serializer.h"

#include <memory>
#include <iostream>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/read_until.hpp>

namespace server
{

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace sys = boost::system;

template <typename THandler>
class Session : public std::enable_shared_from_this<Session<THandler>>
{
private:
    tcp::socket m_Socket;
    std::string m_Buffer;
    THandler m_Handler;

public:
    template <typename UHandler>
    Session(tcp::socket&& client_socket, UHandler&& client_handler)
        : m_Socket(std::move(client_socket))
        , m_Handler(std::forward<UHandler>(client_handler))
    {}

    void Run()
    {
        asio::post(m_Socket.get_executor(), [self = this->shared_from_this()] { self->Read(); });
    }

private:
    void Read()
    {
        asio::async_read_until(
            m_Socket, asio::dynamic_buffer(m_Buffer), ProtocolSerializer::EndSuffix,
            [self = this->shared_from_this()](auto&& ec, size_t bytes_transferred)
            {
                self->OnRead(std::forward<decltype(ec)>(ec), bytes_transferred);
            });
    }

    void OnRead(const sys::error_code& ec, [[maybe_ununsed]] size_t bytes_transferred)
    {
        if (ec)
        {
            std::cerr << "Reading failed: " << ec << std::endl;
            return;
        }

        m_Handler(ProtocolSerializer::Deserialize(std::move(m_Buffer)).Message, [this](std::string&& msg) { this->Write(std::move(msg)); });
    }

    void Write(const std::string& sent_msg)
    {
        asio::async_write(
            m_Socket, asio::buffer(sent_msg), asio::transfer_exactly(sent_msg.size()),
            [self = this->shared_from_this()](auto&& ec, size_t bytes_transferred)
            {
                self->OnWrite(std::forward<decltype(ec)>(ec), bytes_transferred);
            });
    }

    void OnWrite(const sys::error_code& ec, [[maybe_ununsed]] size_t bytes_transferred)
    {
        if (ec)
        {
            std::cerr << "Writing failed: " << ec << std::endl;
            return;
        }
    }
};

template <typename THandler>
class Listener : public std::enable_shared_from_this<Listener<THandler>>
{
private:
    asio::io_context& m_Ioc;
    tcp::acceptor m_Acceptor;
    THandler m_SessionHandler;

public:
    template <typename UHandler>
    Listener(asio::io_context& ioc, const tcp::endpoint& endpoint, UHandler&& handler)
        : m_Ioc(ioc)
        , m_Acceptor(asio::make_strand(m_Ioc))
        , m_SessionHandler(std::forward<UHandler>(handler))
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
        m_Acceptor.async_accept(
            asio::make_strand(m_Ioc),
            [self = this->shared_from_this()](auto&& ec, auto&& client_socket)
            {
                self->OnAccept(std::forward<decltype(ec)>(ec), std::forward<decltype(client_socket)>(client_socket));
            });
    }

    void OnAccept(const sys::error_code& ec, tcp::socket client_socket)
    {
        if (ec)
        {
            std::cerr << "Accept: " << ec << std::endl;
            return;
        }

        AsyncRunSession(std::move(client_socket));

        DoAccept();
    }

    void AsyncRunSession(tcp::socket&& client_socket)
    {
        std::make_shared<Session<THandler>>(std::move(client_socket), m_SessionHandler)->Run();
    }
};

template <typename THandler>
void LaunchServer(asio::io_context& ioc, const tcp::endpoint& endpoint, THandler&& handler)
{
    std::make_shared<Listener<std::decay_t<THandler>>>(ioc, endpoint, std::forward<THandler>(handler))->Run();
}

}
