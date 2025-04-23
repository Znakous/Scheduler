#pragma once

#include <my_utility.h>
#include <my_future.h>

namespace MyConcepts {


template<typename T, typename... Args>
concept InvocableImpl = ((!is_class_member_v<T>) && requires(T&& ac, Args&&... args) {
    Forward<T>(ac)(Forward<Args>(args)...);
}) || (is_class_member_v<T>);

template<typename T, typename... Args>
concept Invocable = InvocableImpl<T, Pure_t<Args>...>;

} // namespace MyConcepts


