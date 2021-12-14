#pragma once

#ifndef YK_TAG_INVOKE_HPP
#define YK_TAG_INVOKE_HPP

#include <concepts>
#include <type_traits>
#include <utility>

namespace yk {

namespace tag_invoke_fn_ns {

template <class Tag, class... Args>
auto tag_invoke(Tag, Args...) = delete;  // poison pill

struct tag_invoke_fn {
  template <class Tag, class... Args>
  constexpr auto operator()(Tag&& tag, Args&&... args) const noexcept(noexcept(tag_invoke(std::forward<Tag>(tag), std::forward<Args>(args)...)))
      -> decltype(tag_invoke(std::forward<Tag>(tag), std::forward<Args>(args)...)) {
    return tag_invoke(std::forward<Tag>(tag), std::forward<Args>(args)...);
  }
};

}  // namespace tag_invoke_fn_ns

inline namespace tag_invoke_ns {

inline constexpr tag_invoke_fn_ns::tag_invoke_fn tag_invoke{};

}  // namespace tag_invoke_ns

template <auto& Tag>
using tag_t = std::decay_t<decltype(Tag)>;

template <class Tag, class... Args>
concept tag_invocable = std::invocable<decltype(tag_invoke), Tag, Args...>;

template <class Tag, class... Args>
concept nothrow_tag_invocable = tag_invocable<Tag, Args...> && std::is_nothrow_invocable_v<decltype(tag_invoke), Tag, Args...>;

template <class Tag, class... Args>
using tag_invoke_result = std::invoke_result<decltype(tag_invoke), Tag, Args...>;

template <class Tag, class... Args>
using tag_invoke_result_t = std::invoke_result_t<decltype(tag_invoke), Tag, Args...>;

}  // namespace yk

#endif  // !YK_TAG_INVOKE_HPP
