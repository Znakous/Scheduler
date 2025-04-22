#pragma once

#include <memory>

#include <my_tuple.h>
#include <my_utility.h>
#include <my_func.h>


template<typename Func, typename... Args>
struct Task {
    Task(Func&& function, const Args&... arguments)
     : function_( new MyFunc::FuncDerived<Func, Args...>(std::forward<Func> (function))), arguments_(arguments...) {
    }

    using function_type = Func;

    template<size_t ind>
    auto GetArg() {
        return arguments_.template Get<ind>();
    }
    
    auto Execute() {
        auto casted = dynamic_cast<MyFunc::FuncDerived<Func, Args...>*> (function_);
        return Apply(casted, arguments_);
    }

private:
    MyFunc::FuncBase<Func, Args...>* function_;
    Tuple<Args...> arguments_;
};