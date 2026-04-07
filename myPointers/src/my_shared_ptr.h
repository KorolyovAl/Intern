#pragma once
#include "control_block.h"

namespace my_ptr {

template<typename T>
class SharedPtr {
public:
    SharedPtr(T* ptr = nullptr) {
        if (ptr == nullptr) {
            return;
        }

        try {            
            cb_ = new detail::ControlBlock{};
            data_ = ptr;
            cb_->shared_count = 1;
            cb_->weak_count = 0;
        }
        catch (...) {
            delete ptr;
            throw;
        }
    }

    SharedPtr(const SharedPtr& other) {
        copy_ptr(other.data_, other.cb_);
    }

    SharedPtr(T* ptr, detail::ControlBlock* cb) {
        copy_ptr(ptr, cb);
    }

    SharedPtr(SharedPtr&& other) noexcept {
        data_ = other.data_;
        cb_ = other.cb_;

        other.data_ = nullptr;
        other.cb_ = nullptr;
    }

    ~SharedPtr() {
        release_current();
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (this == &other) {
            return *this;
        }

        release_current();
        copy_ptr(other.data_, other.cb_);

        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        release_current();

        data_ = other.data_;
        cb_ = other.cb_;

        other.data_ = nullptr;
        other.cb_ = nullptr;

        return *this;
    }

    T& operator*() const noexcept {
        return *data_;
    }

    T* operator->() const noexcept {
        return data_;
    }

    explicit operator bool() const noexcept {
        return data_ != nullptr;
    }

    void reset(T* ptr = nullptr) {
        if (ptr == data_) {
            return;
        }

        detail::ControlBlock* new_cb = nullptr;

        if (ptr != nullptr) {
            try {
                new_cb = new detail::ControlBlock{};
                new_cb->shared_count = 1;
                new_cb->weak_count = 0;
            } catch (...) {
                delete ptr;
                throw;
            }
        }

        release_current();

        data_ = ptr;
        cb_ = new_cb;
    }
    
    T* get() const noexcept {        
        return data_;
    }

    size_t use_count() const {
        return cb_ ? cb_->shared_count : 0;
    }

private:
    void copy_ptr(T* ptr, detail::ControlBlock* cb) {
        data_ = ptr;
        cb_ = cb;

        if (cb_ != nullptr) {
            ++cb_->shared_count;
        }
    }

    void release_current() {
        if (cb_ == nullptr) {
            return;
        }

        --cb_->shared_count;

        if (cb_->shared_count == 0) {
            delete data_;

            if (cb_->weak_count == 0) {
                delete cb_;
            }
        }

        data_ = nullptr;
        cb_ = nullptr;
    }

private:
    T* data_ = nullptr;
    detail::ControlBlock* cb_ = nullptr;

    template <typename>
    friend class WeakPtr;
};

} // namespace my_ptr