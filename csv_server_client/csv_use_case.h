#pragma once

#include <queue>
#include <string>

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
    static size_t Process(const CSVReader::csv_container& doc);
};
