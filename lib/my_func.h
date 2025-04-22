#pragma once

#include <memory>

#include <my_concepts.h>

namespace MyFunc {



template<typename Func, typename... Args>
requires MyConcepts::Invocable<Func, Args...>
struct FuncBase {
    virtual ~FuncBase() {};
    virtual decltype(std::declval<Func>()(Args()...)) operator()(const Args&...) = 0; 
};

template<typename Func, typename... Args>
requires MyConcepts::Invocable<Func, Args...>
struct FuncDerived : FuncBase<Func, Args...> {
    FuncDerived(const Func& func) : func_(&func) {}
    ~FuncDerived() = default;
    decltype(std::declval<Func>()(Args()...)) operator()(const Args&... args) override {
        return (*func_)(args...);
    } 
    const Func* func_;
};

} // namespace MyFunc