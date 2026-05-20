#include <iostream>
#include <sstream>

#include "students.h"

int main() {

    Students students;

    std::stringstream ss;
    ss << "A - 5\n" << "C - 2\n" << "Z - 3\n" << "B - 4\n";

    students.ReadFromFile(ss);
    students.PrintStudents(std::cout);

    std::cout << "---------------\n";

    students.PrintMaximumRatingStudent(std::cout);

    std::cout << "---------------\n";

    std::cout << students.CalculateAverageRating() << std::endl;

    return 0;
}