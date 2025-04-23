#pragma once

#include <my_utility.h>

template<typename T, typename U>
class FutureResult
{
public:
    using ptr_type = T*;
    
    FutureResult(const ptr_type& result, const U& other_iter)
     : result_(result), other_iter_(other_iter)
    {}

    operator T() {
        return *result_;
    }

    auto GetIter() { return other_iter_; }
    
private:
    U other_iter_;
    T* result_;
};

template<typename T>
auto GetArgument(T&& val) {
    return Forward<T> (val);
}

template<typename T, typename U>
auto GetArgument(FutureResult<T, U> val) {
    return static_cast<T>(val);
}


template<typename T>
struct Pure
{
    using type = T;
};

template<typename T, typename U>
struct Pure<FutureResult<T, U>>
{
    using type = Clear_t<T>;
};

template<typename T>
using Pure_t = Pure<T>::type;