#include "log_filter.h"

#include <cctype>
#include <cstdint>
#include <charconv>
#include <bit>

namespace log_filter {

namespace {
using namespace log_filter::detail;

enum class ConditionType {
    CIDR,
    Range
};

std::string_view RemoveExtremeSpaces(std::string_view str) {
    std::string exclude = " \t\r\n";
    size_t start = str.find_first_not_of(exclude);

    if (start == std::string_view::npos) {
        return {};
    }

    size_t end = str.find_last_not_of(exclude);
    
    return str.substr(start, end - start + 1);
}

uint32_t PrefixToMask(uint32_t prefix) {
    if (prefix == 0) {
        return 0;
    }

    // if prefix has more 32 symbols, it's invalid case
    if (prefix >= 32) {
        return 0xFFFFFFFFu;
    }

    return 0xFFFFFFFFu << (32 - prefix);
}

std::string IpV4ToString(uint32_t ip) {
    std::string res = std::to_string((ip >> 24) & 0xFF) + "." 
                    + std::to_string((ip >> 16) & 0xFF) + "."
                    + std::to_string((ip >> 8) & 0xFF) + "."
                    + std::to_string(ip & 0xFF);

    return res;
}

// parsing the IPv4 from string to uint32, a mask is optional
std::pair<uint32_t, std::optional<uint32_t>> ParseIpV4(std::string_view str) {
    bool has_mask = false;
    std::vector<uint32_t> parts;
    std::string buf;

    auto lb_add_num = [&parts, &buf]() {
        if (buf.empty()) {
            throw std::invalid_argument("incorrect IP");
        }

        uint32_t value = 0;
        const char* begin = buf.data();
        const char* end = buf.data() + buf.size();

        auto [ptr, ec] = std::from_chars(begin, end, value);
        if (ec != std::errc{} || ptr != end || value > 255) {
            throw std::invalid_argument("incorrect IP");
        }

        parts.push_back(value);
        buf.clear();
    };

    for (auto ch : str) {
        if (ch == '.') {
            lb_add_num();
            continue;
        }

        if (ch == '/') {
            if (has_mask == true) {
                throw std::invalid_argument("incorrect IP");
            }

            has_mask = true;
            lb_add_num();
            continue;
        }

        if (!std::isdigit(ch)) {
            throw std::invalid_argument("incorrect IP");
        }

        buf.push_back(ch);
    }

    lb_add_num();

    if (has_mask == true && parts.size() != 5) {
        throw std::invalid_argument("incorrect IP");
    }
    else if (has_mask == false && parts.size() != 4) {
        throw std::invalid_argument("incorrect IP");
    }

    uint32_t ip = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
    
    if (has_mask) {
        if (parts[4] > 32) {
            throw std::invalid_argument("incorrect mask");
        }

        return {ip, PrefixToMask(parts[4])};
    }
    return {ip, std::nullopt};
}

// parsing the value of condition
log_filter::Condition ParseValue(std::string_view str, const ConditionType& type) {
    if (type == ConditionType::CIDR) {
        auto [ip, mask] = ParseIpV4(str);
        if (!mask.has_value()) {
            throw std::invalid_argument("incorrect condition: CIDR");
        }

        return ConditionCIDR{.ip_cidr = ip,
                             .mask_cidr = mask.value()
                            };
    }
    
    if (type == ConditionType::Range) {
        size_t pos = str.find('-');
        if (pos == std::string_view::npos) {
            throw std::invalid_argument("incorrect condition: Range");
        }

        auto [ip1, mask1] = ParseIpV4(str.substr(0, pos));
        auto [ip2, mask2] = ParseIpV4(str.substr(pos + 1));

        if (ip1 > ip2) {
            throw std::invalid_argument("incorrect condition: Range");
        }

        if (mask1.has_value() || mask2.has_value()) {
            throw std::invalid_argument("incorrect condition: Range");
        }

        return ConditionRange{.start_range = ip1, 
                              .end_range = ip2
                            };
    }

    throw std::invalid_argument("unknown condition type");
}

log_filter::Condition ParseCondition(const std::vector<std::string>& vec) {
    if (vec.empty()) {
        throw std::invalid_argument("invalid condition: empty condition");
    }

    if (vec.size() % 2 != 0) {
        throw std::invalid_argument("invalid condition: must match the <type> - <value> pattern");
    }

    std::optional<ConditionType> type;
    std::optional<std::string> value;
    for (size_t i = 0; i < vec.size() - 1; ++i) {
        if (vec[i] == "type") {
            ++i;
            if (vec[i] == "subnet") {
                type = ConditionType::CIDR;
            }
            else if (vec[i] == "range") {
                type = ConditionType::Range;
            }
            else {
                throw std::invalid_argument("invalid condition: unknown condition type: " + vec[i]);
            }
        }
        else if (vec[i] == "value") {
            ++i;
            value = vec[i];
        }
        else {
            throw std::invalid_argument("invalid condition: unknown type: " + vec[i]);
        }
    }

    if (!type.has_value()) {
        throw std::invalid_argument("invalid condition: missing type");
    }
    if (!value.has_value()) {
        throw std::invalid_argument("invalid condition: missing value");
    }

    return ParseValue(value.value(), type.value());
}

} // namespace

namespace detail {

bool ConditionCIDR::Matches(uint32_t ip) const {
    uint32_t one = (ip & mask_cidr);
    uint32_t two = (ip_cidr & mask_cidr);
    return one == two;
}

bool ConditionRange::Matches(uint32_t ip) const {
    return start_range <= ip && ip <= end_range;
}

void Rule::AddCondition(Condition&& cnd) {
    conditions_.push_back(std::move(cnd));
}

// check if IP matches to any condition -> return true 
bool Rule::Matches(uint32_t ip) const {
    if (conditions_.empty()) {
        return true;
    }

    bool result = false;
    for (auto& c : conditions_) {
        if (std::holds_alternative<ConditionCIDR>(c)) {
            result = std::get<ConditionCIDR>(c).Matches(ip);
        }
        if (std::holds_alternative<ConditionRange>(c)) {
            result = std::get<ConditionRange>(c).Matches(ip);
        }

        if (result == true) {
            return true;
        }
    }

    return result;
}

} // namespace log_filter::detail

Filter::Filter(const std::vector<std::vector<std::string>>& conditions) {
    for (size_t i = 0; i < conditions.size(); ++i) {
        if (i >= max_conditions_size_) {
            throw std::invalid_argument("maximum conditions number is " + std::to_string(max_conditions_size_));
        }

        Condition c = ParseCondition(conditions[i]);
        rule_.AddCondition(std::move(c));
    }
}

void Filter::StartProcessing(std::istream& input, std::ostream& output) {
    // read line from input
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line.front() == '#') {
            line.clear();
            continue;
        }

        // parsing the line
        std::optional<LogRecord> log;
        try {
            log = ParseAndCheckLine(line);            
        }
        catch (const std::exception&) {
            line.clear();
            continue;
        }
        
        if (!log.has_value()) {
            line.clear();
            continue;
        }

        // put the line to buffer
        buffer_.push_back(std::move(log.value()));

        // if buffer going to limit -> put buffer to output
        if (buffer_.size() == max_buffer_size_) {
            BufferToOutput(output);
        }
    }

    BufferToOutput(output);
}

std::optional<LogRecord> Filter::ParseAndCheckLine(std::string_view line) {
    // parse line
    size_t pos = line.find(" - ");

    std::string ip_part;
    std::string comment_part;

    if (pos == std::string::npos) {
        ip_part = RemoveExtremeSpaces(line);
        comment_part = {};
    }
    else {
        ip_part = RemoveExtremeSpaces(line.substr(0, pos));
        comment_part = RemoveExtremeSpaces(line.substr(pos + 3));
    }

    // parse ip to num
    auto [ip_num, mask] = ParseIpV4(ip_part);
    if (mask.has_value()) {
        throw std::invalid_argument("mask is not allowed in log line");
    }
    
    // check ip for conditions and return
    if (rule_.Matches(ip_num)) {
        return LogRecord{
                            .ip = ip_num,
                            .comment = comment_part
                        };
    }

    return std::nullopt;
}

void Filter::BufferToOutput(std::ostream& output) {
    for (auto& log : buffer_) {
        std::string line = IpV4ToString(log.ip);
        output << line;
        if (!log.comment.empty()) {
            output << " - " << log.comment;
        }

        output << '\n';
    }

    buffer_.clear();
}

} // namespace log_filter

log_filter::Filter create_filter(const std::vector<std::vector<std::string>>& conditions) {
    return log_filter::Filter{conditions};
}

void process_stream(std::istream& input, std::ostream& output, log_filter::Filter& filter) {
    filter.StartProcessing(input, output);
}