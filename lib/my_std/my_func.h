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

    auto operator()(const Args&... args) {
        return (*func_)(GetArgument(args)...);
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

    template<typename Executor>
    auto operator()(Executor* entity, Clear_t<Args>&&... args) {
        return (entity->*(*func_))(GetArgument(args)...);
    } 

    template<typename Executor>
    auto operator()(const Executor* entity, Clear_t<Args>&... args) {
        return (entity->*(*func_))(GetArgument(args)...);
    } 

private:
    const Func* func_;
};

} // namespace MyFunc