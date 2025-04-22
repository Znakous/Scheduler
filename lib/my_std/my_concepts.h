#pragma once

#include <utility>

namespace MyConcepts
{

// template<typename T>
// concept method() {
//     requires is_class_member_v<T>;
// };

template<typename T, typename... Args>
concept Invocable = ((!is_class_member_v<T>) && requires(T&& ac, Args&&... args) {
    std::forward<T>(ac)(std::forward<Args>(args)...);
})
 || ((is_class_member_v<T>) && requires() {true;});

} // namespace MyConcepts


