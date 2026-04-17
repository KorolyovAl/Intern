#include "log_filter.h"

#include "filter_impl.h"

namespace log_filter {

Filter::Filter(const std::vector<std::vector<std::string>>& conditions) {
    impl_ = std::make_unique<FilterImpl>(conditions);
}

Filter::~Filter() = default;

void Filter::StartProcessing(std::istream& input, std::ostream& output) {
    impl_->StartProcessing(input, output);
}

} // namespace log_filter

log_filter::Filter create_filter(const std::vector<std::vector<std::string>>& conditions) {
    return log_filter::Filter{conditions};
}

void process_stream(std::istream& input, std::ostream& output, log_filter::Filter& filter) {
    filter.StartProcessing(input, output);
}