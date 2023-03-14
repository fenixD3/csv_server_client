#pragma once

#include "csv_use_case.h"
#include "shared_file.h"
#include "protocol_serializer.h"

#include <memory>

namespace server
{

class RequestHandler
{
private:
    std::unique_ptr<shared_file::SharedFileManager> m_SharedLog;

public:
    template <typename TSendFunc>
    void operator()(std::string&& input_text, TSendFunc&& send_func)
    {
        TransferredMessage to_sending;
        try
        {
            if (!m_SharedLog)
            {
                m_SharedLog = std::make_unique<shared_file::SharedFileManager>();
            }

            auto csv_result = CSVUseCase::Process(CSVReader::ReadCSV(std::move(input_text)));
            m_SharedLog->Write(csv_result.StealMaxDate(), csv_result.GetQuotient());

            to_sending.Indicator = TransferredMessage::TypeIndicator::DATA;
            to_sending.Message = std::to_string(csv_result.GetDocSize());
        }
        catch (const std::exception& ex)
        {
            to_sending.Indicator = TransferredMessage::TypeIndicator::ERROR;
            to_sending.Message = ex.what();
        }
        send_func(ProtocolSerializer::Serialize(to_sending));
    }
};

}
