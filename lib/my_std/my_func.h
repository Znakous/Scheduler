#pragma once

#include <memory>

#include <my_concepts.h>

namespace MyFunc {

template<typename Func, typename... Args>
requires MyConcepts::Invocable<Func, Args...>
struct FuncDerived {
    FuncDerived(const Func& func) : func_(&func) {}
    auto operator()(const Args&... args) {
        return (*func_)(args...);
    } 
    const Func* func_;
};

template<typename Func, typename... Args>
requires MyConcepts::Invocable<Func, Args...>
struct ClassMethodDerived {
    ClassMethodDerived(const Func& func) : func_(&func) {}

    template<typename Executor>
    auto operator()(Executor* entity, const Args&... args) {
        return (entity->*(*func_))(args...); // страшно, но так реально надо
    } 
    template<typename Executor>
    auto operator()(const Executor* entity, const Args&... args) {
        return (entity->*(*func_))(args...);
    } 
    const Func* func_;
};

} // namespace MyFunc