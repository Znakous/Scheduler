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

private:
    base_type base_;
    std::list<SchedulerIterator> dependencies_;
};


// template<typename... Args>
