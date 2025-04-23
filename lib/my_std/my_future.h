#pragma once

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
    return std::forward<T> (val);
}

template<typename T, typename U>
auto GetArgument(FutureResult<T, U> val) {
    return static_cast<T>(val);
}
