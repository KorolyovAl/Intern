#pragma once
#include "my_shared_ptr.h"

namespace my_ptr {

template<typename T>
class WeakPtr {
public:
    WeakPtr() {}

    WeakPtr(const SharedPtr<T>& shared) {
        copy_ptr(shared.data_, shared.cb_);
    }

    WeakPtr(const WeakPtr& other) {
        copy_ptr(other.data_, other.cb_);
    }

    WeakPtr(WeakPtr&& other) {
        data_ = other.data_;
        cb_ = other.cb_;

        other.data_ = nullptr;
        other.cb_ = nullptr;
    }
    
    ~WeakPtr() {
        release_current();
    }

    WeakPtr& operator=(const SharedPtr<T>& shared) {
        release_current();
        copy_ptr(shared.data_, shared.cb_);

        return *this;
    }

    WeakPtr& operator=(const WeakPtr& other) {
        if (this == &other) {
            return *this;
        }

        release_current();
        copy_ptr(other.data_, other.cb_);

        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
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
    
    SharedPtr<T> lock() const {
        if (cb_ == nullptr || cb_->shared_count == 0) {
            return SharedPtr<T>{};
        }

        return SharedPtr<T>{data_, cb_};
    }

    bool expired() const {
        return use_count() == 0 ? true : false;
    }

    size_t use_count() const {
        return cb_ ? cb_->shared_count : 0;
    }

private:
    void copy_ptr(T* ptr, detail::ControlBlockBase* cb) {
        data_ = ptr;
        cb_ = cb;

        if (cb_ != nullptr) {
            ++cb_->weak_count;
        }
    }

    void release_current() {
        if (cb_ == nullptr) {
            return;
        }

        --cb_->weak_count;
        if (cb_->shared_count == 0 && cb_->weak_count == 0) {
            delete cb_;
        }

        data_ = nullptr;
        cb_ = nullptr;
    }

private:
    T* data_ = nullptr;
    detail::ControlBlockBase* cb_ = nullptr;
};

} // namespace my_ptr