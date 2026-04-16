#pragma once
#include <cstddef>

namespace detail {

struct ControlBlockBase {
    size_t shared_count = 1;
    size_t weak_count = 0;

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