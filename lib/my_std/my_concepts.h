#pragma once

#include <utility>

#include <my_future.h>

namespace MyConcepts {


template<typename T, typename... Args>
concept InvocableImpl = ((!is_class_member_v<T>) && requires(T&& ac, Args&&... args) {
    std::forward<T>(ac)(std::forward<Args>(args)...);
}) || (is_class_member_v<T>);

template<typename T, typename... Args>
concept Invocable = InvocableImpl<T, Pure_t<Clear_t<Args>>...>;

} // namespace MyConcepts


