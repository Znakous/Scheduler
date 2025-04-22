#pragma once

#include <list>
#include <memory>

#include <task.h>
#include <scheduler_iter.h>



class TTaskScheduler 
{
public:
    template<typename T, typename... Args>
    SchedulerIterator add(T&& t, Args&&... args) {
        tasks_.emplace_back(UniquePtrTask(std::forward<T> (t), std::forward<Args> (args)...));
        return --(tasks_.end());
    }

    template<typename T>
    auto getFutureResult(const SchedulerIterator& iter){
        auto res_place = iter->GetResult();
        return FutureResult<T> (res_place.Get<T*>().value());
    }

    template<typename T>
    auto getResult(SchedulerIterator a) {
        auto result = a->Execute().template Get<T>();
        if (result) {
            return result.value();
        } else {
            throw std::exception("task execution failed\n");
        }
    }


    void executeAll() {
        for (auto& task : tasks_) {
            task->Execute();
        }
    }

private:
    std::list<std::unique_ptr<TaskBase>> tasks_;
};
