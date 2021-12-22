//
// Created by Preet Nakrani on 2021-12-05.
//

#ifndef THEDOLLHOUSE_SYNCHRONIZATION_H
#define THEDOLLHOUSE_SYNCHRONIZATION_H
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

class synchronization {
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long count_ = 1; // Initialized as un-locked.

public:
    void release() {
        std::lock_guard<decltype(mutex_)> lock(mutex_);
        ++count_;
        condition_.notify_one();
    }

    void acquire() {
        std::unique_lock<decltype(mutex_)> lock(mutex_);
        while(!count_) // Handle spurious wake-ups.
            condition_.wait(lock);
        --count_;
    }

    bool try_acquire() {
        std::lock_guard<decltype(mutex_)> lock(mutex_);
        if(count_) {
            --count_;
            return true;
        }
        return false;
    }
};

extern synchronization syncRegistry;
extern synchronization syncBattle;

#endif //THEDOLLHOUSE_SYNCHRONIZATION_H
