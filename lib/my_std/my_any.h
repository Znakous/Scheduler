#pragma once

#include <memory>

#include <my_optional.h>
#include <my_utility.h>


struct AnyBase
{
    virtual const void* Get() = 0;
    virtual ~AnyBase() = default;
};

template <typename T>
class AnyTempImpl : public AnyBase 
{
public:
    AnyTempImpl(const T& data)
     : data_(data) 
    {}

    virtual const void* Get() override {
        return static_cast<const void*> (&data_);
    }
    
private:
    T data_;
};


class Any
{
public:
    template<typename T>
    Any(T&& val)
     : any_impl_(std::make_unique<AnyTempImpl<Clear_t<T>>>(std::forward<T>(val))) 
    {}

    template<typename T>
    Optional<T> Get() {
        if (dynamic_cast<AnyTempImpl<T>*> (any_impl_.get())) {
            return *(static_cast<T*> (const_cast<void*>(any_impl_.get()->Get())));
        }
        return Optional<T>();
    }

    template<typename T>
    requires IsConst_v<T>
    Optional<T> Get() {
        if (dynamic_cast<AnyTempImpl<T>*> (any_impl_.get())) {
            return *(static_cast<T*> (any_impl_.get()->Get()));
        }
        return Optional<T>();
    }

private:
    std::unique_ptr<AnyBase> any_impl_;
};
