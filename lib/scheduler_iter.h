#pragma once

#include <list>
#include <memory>

#include <task.h>


class SchedulerIterator
{
public:
    using base_type = std::list<std::unique_ptr<TaskBase>>::iterator;

    SchedulerIterator(base_type base) : base_(base){}

    auto& operator*() const {return **base_; }

    auto& operator->() const {return *base_; }

    void AddDependency(const SchedulerIterator& parent) {
        dependencies_.push_back(parent);
    }

    void ExecuteDependent() {
        for (auto& parent : dependencies_) {
            parent.ExecuteDependent();
        }
        (*base_)->Execute();
    }
    
private:
    base_type base_;
    std::list<SchedulerIterator> dependencies_;
};


template<typename T>
void TryAddDependency(SchedulerIterator& iter, T&& parent) {}

template<typename T>
void TryAddDependency(SchedulerIterator& iter, FutureResult<T, SchedulerIterator> parent) {
    iter.AddDependency(parent.GetIter());
}
