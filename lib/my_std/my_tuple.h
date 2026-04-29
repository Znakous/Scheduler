#pragma once 

#include <stdexcept>

template <typename... Others>
struct Tuple;


template <typename Primary, typename... Others>
struct Tuple<Primary, Others...> 
{
    Tuple(const Primary& this_val_a, const Others&... others_a)
     : this_val(this_val_a), others(Tuple<Others...>(others_a...)) 
    {}

    static constexpr size_t size = 1 + sizeof...(Others);

    template<size_t ind>
    auto Get() {
        return others.template Get<ind-1>();
    }

    template<>
    auto Get<0uz>() {
        return this_val;
    }

    Primary this_val;
    Tuple<Others...> others;
};

template<>
struct Tuple<> 
{
    Tuple() {}

    using Types = void;

    static constexpr size_t size = 0;


    template<size_t ind>
    int Get() {
        throw std::out_of_range("index outside of tuple bounds");
    }
};