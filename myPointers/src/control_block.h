#pragma once

#include <cstddef>
#include <atomic>

namespace detail {

struct ControlBlockBase {
    std::atomic<size_t> shared_count = 1;
    std::atomic<size_t> weak_count = 0;

    virtual void DestroyObject() noexcept = 0;
    virtual ~ControlBlockBase() = default;
};

template<typename U>
struct ControlBlock final : ControlBlockBase {
    explicit ControlBlock(U* ptr) noexcept : ptr_(ptr) {}

    void DestroyObject() noexcept override {
        delete ptr_;
        ptr_ = nullptr;
    }

private:
    U* ptr_ = nullptr;
};

} // namespace detail