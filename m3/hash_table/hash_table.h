#pragma once

#include <iostream>
#include <list>
#include <vector>
#include <string>

template <typename K, typename V>
class HashTable {
    struct Entry {
        K key;
        V value;
        Entry(const K& k, const V& v) : key(k), value(v) {}
    };

public:
    HashTable(size_t cap) : capacity_(cap) {
        table_.resize(capacity_);
    }

    void insert(const K& key, const V& value) {
        size_t index = hash(key);

        for (auto& entry : table_[index]) {
            if (entry.key == key) {
                entry.value = value;
                return;
            }
        }

        table_[index].emplace_back(key, value);
        ++size_;

        if (size_ > 0.75 * capacity_) {
            rehash();
        }
    }

    bool find(const K& key, V& value) const {
        size_t index = hash(key);

        for (auto& entry : table_[index]) {
            if (entry.key == key) {
                value = entry.value;
                return true;
            }
        }

        return false;
    }

    bool remove(const K& key) {
        size_t index = hash(key);

        for (auto it = table_[index].begin(); it != table_[index].end(); ++it) {
            if (it->key == key) {
                table_[index].erase(it);
                size_--;
                return true;
            }
        }

        return false;
    }

    size_t getSize() const { 
        return size_; 
    }

    bool isEmpty() const { 
        return size_ == 0; 
    }

    void printStats() const {
        size_t maxChainLength = 0;

        for (const auto& bucket : table_) {
            size_t chainLength = bucket.size();

            maxChainLength = std::max(maxChainLength, chainLength);
        }

        double averageChainLength = 0.;
        if (!table_.empty()) {
            averageChainLength = static_cast<double>(size_) / table_.size();
        }

        std::cout << "Average chain length: " << averageChainLength << '\n';
        std::cout << "Max chain length: " << maxChainLength << '\n';
    }

private:
    void rehash() {
        size_t oldCapacity = capacity_;
        capacity_ *= 2;

        std::vector<std::list<Entry>> newTable(capacity_);

        for (auto& list : table_) {
            for (auto& entry : list) {
                size_t newIndex = hash(entry.key);
                newTable[newIndex].push_back(entry);
            }
        }

        table_ = std::move(newTable);
    }

    size_t hash(const K& key) {
        std::hash<K> hasher;
        return hasher(key) % capacity_;
    }

private:
    std::vector<std::list<Entry>> table_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};