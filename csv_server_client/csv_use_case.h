#pragma once

#include <queue>
#include <string>
#include <memory>

class CSVReader
{
public:
    class CSVElement
    {
    public:
        std::string Date;
        double FirstNum;
        double SecondNum;

        friend bool operator<(const CSVElement& lhs, const CSVElement& rhs);
        friend std::ostream& operator<<(std::ostream& out, const CSVElement& el);
    };

    using csv_container = std::priority_queue<CSVElement>;

    static csv_container ReadCSV(std::string&& text);
};

class CSVUseCase
{
public:
    class Result
    {
    private:
        std::string m_MaxDate;
        double m_Quotient;
        size_t m_DocSize;

    public:
        Result(std::string max_date, double quotient, size_t doc_size);

        std::string StealMaxDate();
        double GetQuotient() const;
        size_t GetDocSize() const;
    };

    static Result Process(const CSVReader::csv_container& doc);
};
