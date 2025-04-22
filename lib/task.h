#pragma once

#include <memory>

#include <my_tuple.h>
#include <my_utility.h>
#include <my_func.h>
#include <my_optional.h>
#include <my_any.h>
#include <my_future.h>


class ResultHolder 
{
public:
    template<typename T>
    ResultHolder(const T& val) : value_(val) {}

    template<typename T>
    auto Get() {
        return *((value_.template Get<T*>()).value());
    }

private:
    Any value_;
};

struct TaskBase{
    virtual Any Execute() = 0;
    virtual Any GetResult() = 0;
    virtual ~TaskBase() = default;
};


template<typename Func, typename... Args>
requires MyConcepts::Invocable<Func, Args...>
class TaskImpl : public TaskBase 
{
public:
    template<typename F, typename... A>
    TaskImpl(F&& function, A&&... arguments)
     : function_(std::forward<F> (function)), 
       arguments_(std::forward<A> (arguments)...) 
    {}

    using function_type = Func;
    using return_type = decltype(declval<Func>()((declval<Pure_t<Args>>())...));

    template<size_t ind>
    auto GetArg() {
        return arguments_.template Get<ind>();
    }
    
    void Process() {
        if (!result_) {
            result_ = Apply(function_, arguments_);
        }
    }
    
    Any Execute() override {
        if (result_) {
            return result_.value();
        }
        Process();
        return result_.value();
    }

    Any GetResult() override {
        return (result_.value_addr());
    }

    bool Calculated() const {
        return result_;
    }

private:
    MyFunc::FuncStore<Func, Args...> function_;
    Tuple<Args...> arguments_;
    Optional<return_type> result_;
};

template<typename Func, typename... Args>
requires MyConcepts::Invocable<Func, Args...>
auto Task(Func&& function, Args&&... arguments) {
    return TaskImpl<Func, Clear_t<Args>...>(
        std::forward<Func> (function), std::forward<Args> (arguments)...
    );
}

template<typename Func, typename... Args>
requires MyConcepts::Invocable<Func, Args...>
auto UniquePtrTask(Func&& function, Args&&... arguments) {
    return std::make_unique<TaskImpl<Clear_t<Func>, Clear_t<Args>...>>(
        std::forward<Func> (function), std::forward<Args> (arguments)...
    );
}


template<typename Func, typename Executor, typename... Args>
requires is_class_member_v<Func> && MyConcepts::Invocable<Func, Args...>
struct TaskImpl <Func, Executor, Args...> : public TaskBase 
{
    TaskImpl(Func&& function, Executor&& entity, Args&&... arguments)
     : function_(MyFunc::ClassMethodStore<Func, Args...>(std::forward<Func> (function))), 
       arguments_(std::forward<Args> (arguments)...), 
       entity_(const_cast<Clear_t<Executor>*>(&entity))
    {}

    using function_type = Func;
    using return_type = decltype((declval<Executor>().*declval<Func>())(declval<Args>()...));

    template<size_t ind>
    auto GetArg() {
        return arguments_.template Get<ind>();
    }
    
    Any Execute() override {
        if (result_) {
            return result_.value();
        }
        result_ = CallMethod(entity_, function_, arguments_);
        return result_.value();
    }

    Any GetResult() override {
        return result_.value_addr();
    }

    bool Calculated() const {
        return result_;
    }

private:
    Clear_t<Executor>* entity_;
    MyFunc::ClassMethodStore<Func, Args...> function_;
    Tuple<Args...> arguments_;
    Optional<return_type> result_;
};


template<typename Func, typename Executor, typename... Args>
requires is_class_member_v<Func> && MyConcepts::Invocable<Func, Args...>
auto Task(Func&& function, Executor&& executor, Args&&... arguments) {
    return TaskImpl<Func, Executor, Args...> (
            std::forward<Func> (function), 
            std::forward<Executor>(executor), 
            std::forward<Args> (arguments)...
    );
}

template<typename Func, typename Executor, typename... Args>
requires is_class_member_v<Func> && MyConcepts::Invocable<Func, Args...>
auto UniquePtrTask(Func&& function, Executor&& executor, Args&&... arguments) {
    return std::make_unique<TaskImpl<Func, Executor, Args...>>(
            std::forward<Func> (function), 
            std::forward<Executor>(executor), 
            std::forward<Args> (arguments)...
    );
}