#pragma once
#include <cstddef>

namespace detail {

struct ControlBlock {
    size_t shared_count = 1;
    size_t weak_count = 0;
};

} // namespace detail