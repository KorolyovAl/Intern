#pragma once

#include <iostream>
#include <vector>
#include <memory>

namespace log_filter {

class Filter {
public:
    Filter(const std::vector<std::vector<std::string>>& conditions);
    ~Filter();

    void StartProcessing(std::istream& input, std::ostream& output);

private:
    class FilterImpl;
    std::unique_ptr<FilterImpl> impl_;
};

} // namespace log_filter

log_filter::Filter create_filter(const std::vector<std::vector<std::string>>& conditions);
void process_stream(std::istream& input, std::ostream& output, log_filter::Filter& filter);