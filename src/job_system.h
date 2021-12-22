//
// Created by Preet Nakrani on 2021-12-03.
//

#ifndef THEDOLLHOUSE_JOB_SYSTEM_H
#define THEDOLLHOUSE_JOB_SYSTEM_H

#include <map>
#include <thread>
#include <iostream>
#include <functional>
#include <string>

using namespace std;
class job_system {

public:
    uint64_t addJob(const std::function<void()>& func) {
        auto * worker = new thread(func);
        auto id = worker->get_id();
        auto* ptr=(uint64_t*) &id;
        uint64_t final = *ptr;
        job.insert(pair<string, thread*>(to_string(final), worker));
        return final;
    }

    void join(uint64_t id) {
        if (job.find(to_string(id)) != job.end()) {
            if (job[to_string(id)]->joinable()) {
                job[to_string(id)]->join();
            }
            free(job[to_string(id)]);
            job.erase(to_string(id));
        }
    }

private:
   map<string, thread*> job;
};



extern job_system multithreading;
#endif //THEDOLLHOUSE_JOB_SYSTEM_H
