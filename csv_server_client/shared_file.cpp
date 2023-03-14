#include "shared_file.h"

#include <filesystem>
#include <fstream>

namespace shared_file
{

const std::string MappedValue::PositionMemoryName{"position_memory"};
const size_t MappedValue::MemorySize = 3000;

const std::string SharedFileCommon::FilePath{"./protocol"};
constexpr size_t SharedFileCommon::FileSize = 1 * 1024 * 1024;
const std::string SharedFileCommon::MutexName{"shared_mutex"};

ValueWrapper::ValueWrapper(size_t num) : m_Value(num) {}

void ValueWrapper::SetValue(size_t num) { m_Value = num; }

[[nodiscard]] size_t ValueWrapper::GetValue() const { return m_Value; }

MappedValue::MappedValue()
{
    m_Memory = bip::managed_shared_memory(bip::open_or_create,
                                          PositionMemoryName.c_str(),
                                          MemorySize);

    m_Value = m_Memory.find_or_construct<ValueWrapper>(std::string(PositionMemoryName + "_object").c_str())(0);
    if (!m_Value)
    {
        throw bip::interprocess_exception("shm object: find or construct failed");
    }
}

void MappedValue::SetValue(size_t num) { m_Value->SetValue(num); }

[[nodiscard]] size_t MappedValue::GetValue() const { return m_Value->GetValue(); }

SharedFileCommon::SharedFileCommon() : m_Mutex(bip::open_or_create, MutexName.c_str()) {}

void SharedFileCommon::OpenFile()
{
    using namespace std::string_literals;
    if (!std::filesystem::exists(FilePath))
    {
        throw std::runtime_error("File "s + FilePath + " doesn't exist");
    }

    auto mode = bip::read_write;
    auto mapping = bip::file_mapping(FilePath.data(), mode);
    m_FileRegion = bip::mapped_region(mapping, mode);
}

SharedFileManager::SharedFileManager() { PrepareFile(); }

SharedFileManager::~SharedFileManager() { bip::named_mutex::remove(MutexName.c_str()); }

void SharedFileManager::PrepareFile()
{
    if (!std::filesystem::exists(FilePath))
    {
        Create();
    }
    OpenFile();
}

void SharedFileManager::Create()
{
    using namespace std::string_literals;

    auto mode = std::ios_base::out
                | std::ios_base::in
                | std::ios_base::trunc
                | std::ios_base::binary;

    std::filebuf buf;
    buf.open(FilePath.data(), mode);
    if (!buf.is_open())
    {
        throw std::runtime_error("File "s + FilePath + " wasn't opened");
    }

    buf.pubseekoff(FileSize, std::ios_base::beg);
    buf.sputc(0);
    buf.close();
}

SharedFileClient::SharedFileClient()
{
    try
    {
        OpenFile();
    }
    catch (const std::exception& ex)
    {
        throw ex;
    }
}

}
