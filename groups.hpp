#pragma once

#ifndef YK_GROUPS_HPP
#define YK_GROUPS_HPP

#include <concepts>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <version>

#include "tag_invoke.hpp"

#if defined(__cpp_lib_constexpr_string)
#define CXX20_STRING_CONSTEXPR constexpr
#else
#define CXX20_STRING_CONSTEXPR
#endif

#define EITHER(A, B) ((A) and not(B) or not(A) and (B))

namespace yk ::groups {

template <class F, class T, class U>
concept binary_op = std::invocable<F, T, U>;

template <class F, class T>
concept magma = binary_op<F, T, T>;

template <class T>
struct identity_fn {
  template <magma<T> F>
    requires tag_invocable<identity_fn, F>
  constexpr auto operator()(F&& f) const noexcept(nothrow_tag_invocable<identity_fn, F>) -> tag_invoke_result_t<identity_fn, F> {
    return tag_invoke(*this, std::forward<F>(f));
  }
};

template <class T>
inline constexpr identity_fn<T> identity{};

template <class F, class T>
concept unital_impl = magma<F, T> && std::invocable<tag_t<identity<T>>, F>;

template <class T>
struct left_identity_fn {
  template <magma<T> F>
    requires tag_invocable<left_identity_fn, F>
  constexpr auto operator()(F&& f) const noexcept(nothrow_tag_invocable<left_identity_fn, F>) -> tag_invoke_result_t<left_identity_fn, F> {
    return tag_invoke(*this, std::forward<F>(f));
  }

  template <unital_impl<T> F>
  constexpr auto operator()(F&& f) const noexcept(std::is_nothrow_invocable_v<identity_fn<T>, F>) -> std::invoke_result_t<identity_fn<T>, F> {
    return identity<T>(std::forward<F>(f));
  }

  template <unital_impl<T> F>
    requires tag_invocable<left_identity_fn, F>
  constexpr auto operator()(F&& f) const noexcept(std::is_nothrow_invocable_v<identity_fn<T>, F>) -> std::invoke_result_t<identity_fn<T>, F> {
    return identity<T>(std::forward<F>(f));
  }
};

template <class T>
inline constexpr left_identity_fn<T> left_identity{};

template <class F, class T>
concept left_unital_impl = magma<F, T> and std::invocable<tag_t<left_identity<T>>, F>;

template <class T>
struct right_identity_fn {
  template <magma<T> F>
    requires tag_invocable<right_identity_fn, F>
  constexpr auto operator()(F&& f) const noexcept(nothrow_tag_invocable<right_identity_fn, F>) -> tag_invoke_result_t<right_identity_fn, F> {
    return tag_invoke(*this, std::forward<F>(f));
  }

  template <unital_impl<T> F>
  constexpr auto operator()(F&& f) const noexcept(std::is_nothrow_invocable_v<identity_fn<T>, F>) -> std::invoke_result_t<identity_fn<T>, F> {
    return identity<T>(std::forward<F>(f));
  }

  template <unital_impl<T> F>
    requires tag_invocable<right_identity_fn, F>
  constexpr auto operator()(F&& f) const noexcept(std::is_nothrow_invocable_v<identity_fn<T>, F>) -> std::invoke_result_t<identity_fn<T>, F> {
    return identity<T>(std::forward<F>(f));
  }
};

template <class T>
inline constexpr right_identity_fn<T> right_identity{};

template <class F, class T>
concept right_unital_impl = magma<F, T> and std::invocable<tag_t<right_identity<F>>, T>;

template <class F, class T>
concept left_unital = left_unital_impl<F, T> or unital_impl<F, T>;

template <class F, class T>
concept right_unital = right_unital_impl<F, T> or unital_impl<F, T>;

template <class F, class T>
concept unital = (unital_impl<F, T>) or (left_unital_impl<F, T> and right_unital_impl<F, T>);

template <class F, class T>
inline constexpr bool enable_semigroup = false;

template <class F, class T>
concept semigroup = magma<F, T> && enable_semigroup<std::remove_cvref_t<F>, T>;

template <class F, class T>
concept monoid = semigroup<F, T> && unital<F, T>;

inline constexpr struct plus_fn {
  template <class T, class U>
    requires tag_invocable<plus_fn, T, U>
  constexpr auto operator()(T&& x, U&& y) const noexcept(nothrow_tag_invocable<plus_fn, T, U>) -> tag_invoke_result_t<plus_fn, T, U> {
    return tag_invoke(*this, std::forward<T>(x), std::forward<U>(y));
  }

  // arithmeric
  template <class T>
    requires(std::is_arithmetic_v<std::remove_cvref_t<T>>)
  friend constexpr auto tag_invoke(plus_fn, T x, T y) noexcept { return x + y; }

  template <class T>
    requires(std::is_arithmetic_v<std::remove_cvref_t<T>>)
  friend constexpr auto tag_invoke(identity_fn<T>, plus_fn) noexcept -> std::remove_cvref_t<T> { return 0; }

  // string
  template <class CharT, class Traits, class Alloc>
  friend CXX20_STRING_CONSTEXPR auto tag_invoke(plus_fn, std::basic_string<CharT, Traits, Alloc> x, std::basic_string<CharT, Traits, Alloc> y) {
    return x + y;
  }

  template <class CharT, class Traits, class Alloc>
  friend CXX20_STRING_CONSTEXPR auto tag_invoke(identity_fn<std::basic_string<CharT, Traits, Alloc>>, plus_fn) {
    return std::basic_string<CharT, Traits, Alloc>{};
  }

} plus{};

template <class T>
  requires(std::is_arithmetic_v<T>)
inline constexpr bool enable_semigroup<plus_fn, T> = true;

template <class CharT, class Traits, class Alloc>
inline constexpr bool enable_semigroup<plus_fn, std::basic_string<CharT, Traits, Alloc>> = true;

inline constexpr struct minus_fn {
  template <class T, class U>
    requires tag_invocable<minus_fn, T, U>
  constexpr auto operator()(T&& x, U&& y) const noexcept(nothrow_tag_invocable<minus_fn, T, U>) -> tag_invoke_result_t<minus_fn, T, U> {
    return tag_invoke(*this, std::forward<T>(x), std::forward<U>(y));
  }

  template <class T>
    requires(std::is_arithmetic_v<T>)
  friend constexpr auto tag_invoke(minus_fn, T x, T y) noexcept { return x - y; }

  template <class T>
    requires(std::is_arithmetic_v<std::remove_cvref_t<T>>)
  friend constexpr auto tag_invoke(right_identity_fn<T>, minus_fn) noexcept -> std::remove_cvref_t<T> { return 0; }

} minus{};

}  // namespace yk::groups

#endif  // !YK_GROUPS_HPP
