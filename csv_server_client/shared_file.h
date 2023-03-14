#pragma once

#include <sstream>
#include <memory>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

namespace shared_file
{

namespace bip = boost::interprocess;

class ValueWrapper
{
private:
    size_t m_Value;

public:
    ValueWrapper(size_t num);

    void SetValue(size_t num);

    [[nodiscard]] size_t GetValue() const;
};

class MappedValue
{
private:
    static const std::string PositionMemoryName;
    static const size_t MemorySize;

    bip::managed_shared_memory m_Memory;
    ValueWrapper *m_Value;

public:
    MappedValue();

    void SetValue(size_t num);

    [[nodiscard]] size_t GetValue() const;
};

class SharedFileManager;

class SharedFileCommon
{
private:
    friend class SharedFileManager;

    static const std::string FilePath;
    static const size_t FileSize;
    static const std::string MutexName;

protected:
    bip::named_mutex m_Mutex;
    bip::mapped_region m_FileRegion;
    MappedValue m_CurrentPosition;

    SharedFileCommon();

    void OpenFile();

public:
    template <typename ...TArgs>
    void Write(TArgs&&... args)
    {
        using namespace std::string_literals;

        std::stringstream temp_stream;
        ((temp_stream << std::forward<TArgs>(args) << ' '), ...);
        temp_stream << '\n';

        std::lock_guard guard(m_Mutex);

        std::string text = std::move(temp_stream).str();
        if (text.size() + m_CurrentPosition.GetValue() > FileSize)
        {
            throw std::logic_error("File "s + FilePath + " is too big");
        }

        char* addr = static_cast<char*>(m_FileRegion.get_address());
        size_t last_pos = m_CurrentPosition.GetValue();
        for (size_t i = 0; i < text.size(); ++i)
        {
            addr[last_pos + i] = text[i];
        }

        m_CurrentPosition.SetValue(last_pos + text.size());
    }
};

class SharedFileManager : public SharedFileCommon
{
public:
    SharedFileManager();

    ~SharedFileManager();

private:
    void PrepareFile();

    void Create();
};

class SharedFileClient : private SharedFileCommon
{
public:
    SharedFileClient();

    using SharedFileCommon::Write;
};

}
