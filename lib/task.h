#pragma once

#include <memory>

#include <my_tuple.h>
#include <my_utility.h>
#include <my_func.h>
#include <my_optional.h>


template<typename Func, typename... Args>
struct Task {
    Task(Func&& function, const Args&... arguments)
     : function_(MyFunc::FuncDerived<Func, Args...>(std::forward<Func> (function))), arguments_(arguments...) {
    }

    using function_type = Func;
    using return_type = decltype(declval<Func>()((declval<Args>())...));

    template<size_t ind>
    auto GetArg() {
        return arguments_.template Get<ind>();
    }
    
    auto Execute() {
        if (result_) {
            return result_.value();
        }
        result_ = Apply(function_, arguments_);
        return result_.value();
    }

    bool Calculated() const {
        return result_;
    }

private:
    MyFunc::FuncDerived<Func, Args...> function_;
    Tuple<Args...> arguments_;
    Optional<return_type> result_;
};


template<typename Func, typename Executor, typename... Args>
requires is_class_member_v<Func>
struct Task <Func, Executor, Args...> {
    template<typename F, typename E>
    Task(F&& function, E&& entity, const Args&... arguments)
     : function_(MyFunc::ClassMethodDerived<Func, Args...>(std::forward<Func> (function))), arguments_(arguments...), entity_(const_cast<Clear_t<E>*>(&entity)) {
    }

    using function_type = Func;
    using return_type = decltype((declval<Executor>().*declval<Func>())(declval<Args>()...));

    template<size_t ind>
    auto GetArg() {
        return arguments_.template Get<ind>();
    }
    
    auto Execute() {
        if (result_) {
            return result_.value();
        }
        result_ = CallMethod(entity_, function_, arguments_);
        return result_.value();
    }

    bool Calculated() const {
        return result_;
    }

private:
    Clear_t<Executor>* entity_;
    MyFunc::ClassMethodDerived<Func, Args...> function_;
    Tuple<Args...> arguments_;
    Optional<return_type> result_;
};
