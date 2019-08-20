#ifndef __NAME_H_
#define __NAME_H_

#include <string>


namespace dipc {
namespace detail {
template <char... Str>
struct to_const_char_arr {
  static constexpr const char value[] = {Str..., '\0'};
};
}  // namespace detail
}  // namespace dipc

/**
 * string-literal operator
 *
 * @brief "foo"_n representing a constant of type `const char*` and value is "foo"
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
template <typename T, T... Str>
inline constexpr const char* operator""_n() {
  constexpr const char* ss = dipc::detail::to_const_char_arr<Str...>::value;
  return ss;
}
#pragma clang diagnostic pop

#endif  // __NAME_H_
