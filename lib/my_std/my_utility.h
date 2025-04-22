#pragma once

#include <my_tuple.h>

struct false_type { static constexpr bool value = false; };
struct true_type { static constexpr bool value = true; };



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
    return (std::forward<Func>(func))(tuple.template Get<indices>()...);
}

template<typename Func, typename Tuple>
auto Apply(Func&& func, Tuple&& tuple) {
    return ApplyImpl(func, tuple, (typename sequence::MakeSequence<tuple.size>::type){});
}

template<typename Class, typename Func, typename Tuple, int... indices>
auto CallMethodImpl(Class&& entity, Func&& func, Tuple&& tuple, sequence::Sequence<indices...>&&) {
    return ((std::forward<Func>(func)))(entity, tuple.template Get<indices>()...);
}

template<typename Class, typename Func, typename Tuple>
auto CallMethod(Class&& entity, Func&& func, Tuple&& tuple) {
    return CallMethodImpl(entity, func, tuple, (typename sequence::MakeSequence<tuple.size>::type){});
}

template<typename T>
struct is_class_member {
    using value = false_type;
};

template<typename T, class Father>
struct is_class_member< T Father::*> {
    using value = true_type;
};

template<typename T>
constexpr bool is_class_member_v = is_class_member<T>::value::value;


template<typename T>
struct SimpleType {
    using type = T;
};

template<typename T>
struct RemoveRef
{
    using type = T;
};

template<typename T>
struct RemoveRef<T&>
{
    using type = SimpleType<T>::type;
};

template<typename T>
struct RemoveRef<T&&>
{
    using type = SimpleType<T>::type;
};

template<typename T>
using RemoveRef_t = typename RemoveRef<T>::type;

template<typename T>
struct AddRvRef {
    using type = RemoveRef_t<T>&&;
};

template<typename T>
using AddRvRef_t = typename AddRvRef<T>::type;

template<typename T>
AddRvRef_t<T> declval() noexcept {}


template<typename T>
struct RemoveConst
{
    using type = T;
};

template<typename T>
struct RemoveConst<const T>
{
    using type = SimpleType<T>::type;
};

template<typename T>
using RemoveConst_t = RemoveConst<T>::type;

template<typename T>
using Clear_t = RemoveConst_t<RemoveRef_t<T>>;

template<typename T>
constexpr bool IsConst_v = false;

template<typename T>
constexpr bool IsConst_v<const T> = true;

