#include "csv_server_client/csv_use_case.h"

#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

template<typename Q>
void print_queue(std::string_view name, Q q)
{
    // NB: q is passed by value because there is no way to traverse
    // priority_queue's content without erasing the queue.
    for (std::cout << name << ": "; !q.empty(); q.pop())
        std::cout << q.top() << ": ";
    std::cout << '\n';
}

bool operator==(const CSVReader::CSVElement& lhs, const CSVReader::CSVElement& rhs)
{
    return std::tie(lhs.Date, lhs.FirstNum, lhs.SecondNum) == std::tie(rhs.Date, rhs.FirstNum, rhs.SecondNum);
}

template<typename Q>
void CompareQueues(Q q1, Q q2)
{
    EXPECT_EQ(q1.size(), q2.size());

    for (; !q1.empty(); q1.pop(), q2.pop())
    {
        EXPECT_EQ(q1.top(), q2.top());
    }
}

CSVReader::CSVElement CreateExpectedElement(const std::string& date, double first, double second)
{
    return {date, first, second};
}

std::string GenerateCSV(const std::string& date, double first, double second)
{
    return date + ',' + boost::lexical_cast<std::string>(first) + ',' + boost::lexical_cast<std::string>(second) + '\n';
}

TEST(CSVReader, FirstDateGreatherSecond)
{
    std::string date1 = "09.04.2023 12:12:12";
    double first1 = 34.12;
    double second1 = 1.2;

    std::string date2 = "09.04.2023 09:12:12";
    double first2 = 2.2;
    double second2 = 1.1;

    std::string csv = GenerateCSV(date1, first1, second1) + GenerateCSV(date2, first2, second2);

    std::priority_queue<CSVReader::CSVElement> expected;
    expected.push(CreateExpectedElement(date1, first1, second1));
    expected.push(CreateExpectedElement(date2, first2, second2));

    CompareQueues(expected, CSVReader::ReadCSV(std::move(csv)));
}

TEST(CSVReader, FirstDateLessSecond)
{
    std::string date1 = "09.04.2023 09:12:12";
    double first1 = 2.2;
    double second1 = 1.1;

    std::string date2 = "09.04.2023 12:12:12";
    double first2 = 34.12;
    double second2 = 1.2;

    std::string csv = GenerateCSV(date1, first1, second1) + GenerateCSV(date2, first2, second2);

    std::priority_queue<CSVReader::CSVElement> expected;
    expected.push(CreateExpectedElement(date2, first2, second2));
    expected.push(CreateExpectedElement(date1, first1, second1));

    CompareQueues(expected, CSVReader::ReadCSV(std::move(csv)));
}

TEST(CSVReader, TwoEqualedDate)
{
    std::string date1 = "09.04.2023 09:12:12";
    double first1 = 2.2;
    double second1 = 1.1;

    std::string date2 = "09.04.2023 09:12:12";
    double first2 = 34.12;
    double second2 = 1.2;

    std::string csv = GenerateCSV(date1, first1, second1) + GenerateCSV(date2, first2, second2);

    std::priority_queue<CSVReader::CSVElement> expected;
    expected.push(CreateExpectedElement(date1, first1, second1));
    expected.push(CreateExpectedElement(date2, first2, second2));

    CompareQueues(expected, CSVReader::ReadCSV(std::move(csv)));
}

TEST(CSVUseCase, TwoRecords)
{
    std::string date1 = "09.04.2023 09:12:12";
    double first1 = 2.2;
    double second1 = 1.1;

    std::string date2 = "09.04.2023 12:12:12";
    double first2 = 34.12;
    double second2 = 1.2;

    std::string csv = GenerateCSV(date1, first1, second1) + GenerateCSV(date2, first2, second2);

    std::priority_queue<CSVReader::CSVElement> expected;
    expected.push(CreateExpectedElement(date2, first2, second2));
    expected.push(CreateExpectedElement(date1, first1, second1));

    auto read_size = CSVUseCase::Process(CSVReader::ReadCSV(std::move(csv)));
    EXPECT_EQ(expected.size(), read_size);
}
