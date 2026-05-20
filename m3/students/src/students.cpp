#include "students.h"

#include <optional>
#include <string_view>
#include <charconv>
#include <numeric>
#include <algorithm>

namespace {

std::string_view RemoveExtremeSpaces(std::string_view str) {
    std::string exclude = " \t\r\n";
    size_t start = str.find_first_not_of(exclude);

    if (start == std::string_view::npos) {
        return {};
    }

    size_t end = str.find_last_not_of(exclude);
    
    return str.substr(start, end - start + 1);
}

std::optional<Students::PersonData> ParseLine(std::string_view line) {
    size_t pos = line.find(" - ");   

    if (pos == std::string::npos) {
        return std::nullopt;
    }

    std::string name_part;
    std::string rating_part;

    name_part = RemoveExtremeSpaces(line.substr(0, pos));
    rating_part = RemoveExtremeSpaces(line.substr(pos + 3));

    if (name_part.empty() || rating_part.empty()) {
        throw std::invalid_argument("empty student data");
    }

    int rating = 0;
    const char* begin = rating_part.data();
    const char* end = rating_part.data() + rating_part.size();
    auto [ptr, ec] = std::from_chars(begin, end, rating);
    
    if (ec != std::errc{} || ptr != end || rating <= 0) {
        throw std::invalid_argument("invalid student rating");
    }

    return Students::PersonData{
                        .name = name_part,
                        .rating = rating
                    };
}

}

void Students::ReadFromFile(std::istream& input) {
    std::string line;
    while (getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        auto person = ParseLine(line);
        if (person.has_value()) {
            data_[person->name] = person->rating; 
        }
    }
}

void Students::PrintStudents(std::ostream& out) const {
    for (const auto& s : data_) {
        out << s.first << " - " << s.second << '\n';
    }
}

void Students::PrintMaximumRatingStudent(std::ostream& out) const {
    if (data_.empty()) {
        return;
    }

    auto max_it = std::max_element(data_.begin(), data_.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.second < rhs.second;
        }
    );

    out << max_it->first << " - " << max_it->second << '\n';
}

double Students::CalculateAverageRating() const {
    if (data_.empty()) {
        return 0.;
    }
    
    std::int64_t sum = std::accumulate(data_.begin(), data_.end(), std::int64_t{0},
        [](std::int64_t acc, const auto& item) {
            return acc + item.second;
        }
    );

    return static_cast<double>(sum) / data_.size();
}