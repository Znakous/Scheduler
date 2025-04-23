#pragma once

#include <memory>

#include <my_concepts.h>
#include <my_future.h>
#include <my_utility.h>

namespace MyFunc {

template<typename Func, typename... Args>
requires MyConcepts::Invocable<Func, Args...>
class FuncStore 
{
public:
    FuncStore(const Func& func)
     : func_(&func) 
    {}

    template<typename... A>
    auto operator()(A&&... args) {
        return (*func_)(GetArgument(std::forward<A> (args))...);
    } 

private:
    const Clear_t<Func>* func_;
};


template<typename Func, typename... Args>
requires MyConcepts::Invocable<Func, Args...>
class ClassMethodStore 
{
public:
    ClassMethodStore(const Func& func) : func_(&func) {}

    template<typename Executor, typename... A>
    auto operator()(Executor* entity, A&&... args) {
        return (entity->*(*func_))(GetArgument(args)...);
    } 

    template<typename Executor, typename... A>
    auto operator()(const Executor* entity, A&... args) {
        return (const_cast<Executor*>(entity)->*(*func_))(GetArgument(args)...);
    } 

private:
    const Func* func_;
};

} // namespace MyFunc