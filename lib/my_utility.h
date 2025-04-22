#pragma once

#include <my_tuple.h>


template<bool selector, typename T, typename F>
struct conditional{};

template<typename T, typename F>
struct conditional<false, T, F>{ using type = F; };

template<typename T, typename F>
struct conditional<true, T, F>{ using type = T; };


namespace sequence {

template<int... values>
struct Sequence {};


template<int cur, int... all>
struct MakeSequence : MakeSequence<cur - 1, cur - 1, all...> {};

template<int... all>
struct MakeSequence<0, all...>
{
    using type = Sequence<all...>;
};

} // namespace sequence

template<typename Func, typename Tuple, int... indices>
auto ApplyImpl(Func&& func, Tuple&& tuple, sequence::Sequence<indices...>&&) {
    return (*std::forward<Func>(func))(tuple.template Get<indices>()...);
}

template<typename Func, typename Tuple>
auto Apply(Func&& func, Tuple&& tuple) {
    return ApplyImpl(func, tuple, (typename sequence::MakeSequence<tuple.size>::type){});
}