#pragma once

#include <iostream>
#include <string>
#include <map>

class Students {
public:
    struct PersonData {
        std::string name;
        int rating = 0;
    };

    void ReadFromFile(std::istream& input);
    void PrintStudents(std::ostream& out) const ;
    void PrintMaximumRatingStudent(std::ostream& out) const;
    double CalculateAverageRating() const;

private:
    std::map<std::string, int> data_; // name - rating
};