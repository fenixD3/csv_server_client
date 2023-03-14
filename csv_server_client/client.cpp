#include "client.h"
#include "protocol_serializer.h"
#include "shared_file.h"

#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>

#include <thread>

namespace client
{

Client::Client(asio::io_context& ioc, const tcp::endpoint& endpoint, std::string&& text)
    : m_Socket(ioc)
    , m_Endpoint(endpoint)
    , m_TransferringText(std::move(text))
{}

void Client::RunAndConnect()
{
    sys::error_code ec;

    m_Socket.connect(m_Endpoint, ec);

    if (ec)
    {
        std::cerr << "Connection failed: " << ec << std::endl;
        return;
    }
    std::this_thread::sleep_for(std::chrono::seconds(4));
    WriteRequest();
}

void Client::WriteRequest()
{
    sys::error_code ec;
    auto transferring_msg = ProtocolSerializer::Serialize({TransferredMessage::TypeIndicator::DATA, std::move(m_TransferringText)});
    asio::write(m_Socket, asio::buffer(transferring_msg), asio::transfer_exactly(transferring_msg.size()), ec);

    if (ec)
    {
        std::cerr << "Sending failed: " << ec << std::endl;
        return;
    }
    ReadResponse();
}

void Client::ReadResponse()
{
    std::string response;
    sys::error_code ec;

    asio::read_until(m_Socket, asio::dynamic_buffer(response), ProtocolSerializer::EndSuffix, ec);
    if (ec)
    {
        std::cerr << "Receiving failed: " << ec << std::endl;
        return;
    }
    ProcessResponse(ProtocolSerializer::Deserialize(std::move(response)));
}

void Client::ProcessResponse(const TransferredMessage& response)
{
    if (response.Indicator == TransferredMessage::TypeIndicator::ERROR)
    {
        std::cerr << "Error from server: " << response.Message << std::endl;
    }
    else
    {
        try
        {
            shared_file::SharedFileClient output_protocol;
            output_protocol.Write(response.Message);
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Protocol file exception: " << ex.what() << std::endl;
            return;
        }
    }
}

}
