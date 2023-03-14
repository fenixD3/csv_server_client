#include "csv_use_case.h"

#include <sstream>
#include <boost/lexical_cast.hpp>

#include <iostream>

namespace
{

CSVReader::CSVElement GenerateCSVElement(std::string&& csv_line)
{
    std::istringstream stream(std::move(csv_line));
    CSVReader::CSVElement element;

    std::getline(stream, element.Date, ',');

    std::string num;
    std::getline(stream, num, ',');
    element.FirstNum = boost::lexical_cast<double>(num);

    std::getline(stream, num);
    element.SecondNum = boost::lexical_cast<double>(num);

    return element;
}

}

bool operator<(const CSVReader::CSVElement& lhs, const CSVReader::CSVElement& rhs)
{
    return lhs.Date < rhs.Date;
}

std::ostream& operator<<(std::ostream& out, const CSVReader::CSVElement& el)
{
    out << el.Date << ' ' << el.FirstNum << ' ' << el.SecondNum;
    return out;
}

CSVReader::csv_container CSVReader::ReadCSV(std::string&& text)
{
    csv_container csv_lines;
    std::istringstream stream(std::move(text));

    for (std::string line; std::getline(stream, line);)
    {
        csv_lines.push(GenerateCSVElement(std::move(line)));
    }

    return csv_lines;
}

CSVUseCase::Result::Result(std::string max_date, double quotient, size_t doc_size)
    : m_MaxDate(std::move(max_date))
    , m_Quotient(quotient)
    , m_DocSize(doc_size)
{}

std::string CSVUseCase::Result::StealMaxDate()
{
    return std::move(m_MaxDate);
}

double CSVUseCase::Result::GetQuotient() const
{
    return m_Quotient;
}

size_t CSVUseCase::Result::GetDocSize() const
{
    return m_DocSize;
}

CSVUseCase::Result CSVUseCase::Process(const CSVReader::csv_container& doc)
{
    const auto& high_data = doc.top();
    return Result(high_data.Date, high_data.FirstNum / high_data.SecondNum, doc.size());
}
