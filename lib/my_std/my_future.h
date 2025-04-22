#pragma once

template<typename T>
class FutureResult
{
public:
    using ptr_type = T*;
    
    FutureResult(const ptr_type& result)
     : result_(result)
    {}

    operator T() {
        return *result_;
    }

private:
    T* result_;
};

template<typename T>
auto GetArgument(T&& val) {
    return std::forward<T> (val);
}

template<typename T>
auto GetArgument(FutureResult<T> val) {
    return static_cast<T>(val);
}
