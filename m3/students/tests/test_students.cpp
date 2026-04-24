#include <sstream>
#include <stdexcept>
#include <string>

#include <catch2/catch_all.hpp>

#include "students.h"

SCENARIO("ReadFromFile stores students and PrintStudents outputs them in key order") {
    Students students;
    std::stringstream input;
    input << "Charlie - 4\n";
    input << "Alice - 5\n";
    input << "Bob - 3\n";

    students.ReadFromFile(input);

    std::ostringstream output;
    students.PrintStudents(output);

    REQUIRE(output.str() == "Alice - 5\nBob - 3\nCharlie - 4\n");
}

SCENARIO("ReadFromFile overwrites rating for duplicate student names") {
    Students students;
    std::stringstream input;
    input << "Alice - 3\n";
    input << "Alice - 5\n";

    students.ReadFromFile(input);

    std::ostringstream output;
    students.PrintStudents(output);

    REQUIRE(output.str() == "Alice - 5\n");
}

SCENARIO("ReadFromFile ignores lines without expected delimiter") {
    Students students;
    std::stringstream input;
    input << "Alice - 5\n";
    input << "broken line\n";
    input << "Bob - 4\n";

    students.ReadFromFile(input);

    std::ostringstream output;
    students.PrintStudents(output);

    REQUIRE(output.str() == "Alice - 5\nBob - 4\n");
}

SCENARIO("ReadFromFile throws for empty student name or rating") {
    WHEN("empty name") {
        Students students;
        std::stringstream input("  - 5\n");

        REQUIRE_THROWS_AS(students.ReadFromFile(input), std::invalid_argument);
    }

    WHEN("empty rating") {
        Students students;
        std::stringstream input("Alice -   \n");

        REQUIRE_THROWS_AS(students.ReadFromFile(input), std::invalid_argument);
    }
}

SCENARIO("ReadFromFile throws for invalid ratings") {
    WHEN("non numeric rating") {
        Students students;
        std::stringstream input("Alice - five\n");

        REQUIRE_THROWS_AS(students.ReadFromFile(input), std::invalid_argument);
    }

    WHEN("zero rating") {
        Students students;
        std::stringstream input("Alice - 0\n");

        REQUIRE_THROWS_AS(students.ReadFromFile(input), std::invalid_argument);
    }

    WHEN("negative rating") {
        Students students;
        std::stringstream input("Alice - -2\n");

        REQUIRE_THROWS_AS(students.ReadFromFile(input), std::invalid_argument);
    }
}

SCENARIO("PrintMaximumRatingStudent outputs student with maximum rating") {
    Students students;
    std::stringstream input;
    input << "Alice - 4\n";
    input << "Bob - 5\n";
    input << "Charlie - 3\n";

    students.ReadFromFile(input);

    std::ostringstream output;
    students.PrintMaximumRatingStudent(output);

    REQUIRE(output.str() == "Bob - 5\n");
}

SCENARIO("PrintMaximumRatingStudent does nothing for empty collection") {
    Students students;
    std::ostringstream output;

    students.PrintMaximumRatingStudent(output);

    REQUIRE(output.str().empty());
}

SCENARIO("CalculateAverageRating returns average for non empty collection") {
    Students students;
    std::stringstream input;
    input << "Alice - 5\n";
    input << "Bob - 3\n";
    input << "Charlie - 4\n";

    students.ReadFromFile(input);

    REQUIRE(students.CalculateAverageRating() == Catch::Approx(4.0));
}

SCENARIO("CalculateAverageRating returns zero for empty collection") {
    Students students;

    REQUIRE(students.CalculateAverageRating() == Catch::Approx(0.0));
}