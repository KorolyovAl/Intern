#pragma once

#include "log_filter.h"

#include <stdint.h>
#include <variant>
#include <vector>
#include <string>
#include <optional>
#include <iostream>

namespace log_filter {

namespace detail {

struct ConditionCIDR {
    uint32_t ip_cidr;
    uint32_t mask_cidr;

    bool Matches(uint32_t ip) const;
};

struct ConditionRange {
    uint32_t start_range;
    uint32_t end_range;

    bool Matches(uint32_t ip) const;
};

using Condition = std::variant<std::monostate, ConditionCIDR, ConditionRange>;

class Rule {
public:
    void AddCondition(Condition&& cnd);

    // check if IP matches to any condition -> return true 
    bool Matches(uint32_t ip) const;

private:
    std::vector<Condition> conditions_;
};

struct LogRecord {
    uint32_t ip;
    std::string comment;
};

} // namespace detail

class Filter::FilterImpl {
public:
    FilterImpl(const std::vector<std::vector<std::string>>& conditions);

    void StartProcessing(std::istream& input, std::ostream& output);

private: 
    std::optional<detail::LogRecord> ParseAndCheckLine(std::string_view line);
    void BufferToOutput(std::ostream& output);

private:
    detail::Rule rule_;
    std::vector<detail::LogRecord> buffer_;

    const size_t max_buffer_size_ = 1'000;
    const size_t max_conditions_size_ = 20;
};

} // namespace log_filter