#pragma once
#include "control_block.h"

namespace my_ptr {

template<typename T>
class SharedPtr {
private:    
    SharedPtr(T* ptr, detail::ControlBlockBase* cb) {
        copy_ptr(ptr, cb);
    }
public:
    SharedPtr() = default;

    template<typename U>
    SharedPtr(U* ptr = nullptr) {
        if (ptr == nullptr) {
            return;
        }

        try {            
            cb_ = new detail::ControlBlock<U>(ptr);
            data_ = ptr;
        }
        catch (...) {
            delete ptr;
            throw;
        }
    }

    SharedPtr(const SharedPtr& other) {
        copy_ptr(other.data_, other.cb_);
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

    template<typename U>
    void reset(U* ptr = nullptr) {
        if (ptr == data_) {
            return;
        }

        detail::ControlBlockBase* new_cb = nullptr;

        if (ptr != nullptr) {
            try {
                new_cb = new detail::ControlBlock<U>(ptr);
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
    void copy_ptr(T* ptr, detail::ControlBlockBase* cb) {
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
            cb_->DestroyObject();

            if (cb_->shared_count == 0 && cb_->weak_count == 0) {
                delete cb_;
            }
        }

        data_ = nullptr;
        cb_ = nullptr;
    }

private:
    T* data_ = nullptr;
    detail::ControlBlockBase* cb_ = nullptr;

    template <typename>
    friend class WeakPtr;
};

} // namespace my_ptr