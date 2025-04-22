#pragma once

#include <utility>

namespace MyConcepts
{

template<typename T, typename... Args>
concept Invocable = requires(T&& ac, Args&&... args) {
    std::forward<T>(ac)(std::forward<Args>(args)...);
};

} // namespace MyConcepts


