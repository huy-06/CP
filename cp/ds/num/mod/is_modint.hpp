#include <concepts>
#include <type_traits>

#ifndef CP_DS_MODULAR_IS_MODINT
#define CP_DS_MODULAR_IS_MODINT
namespace cp {
namespace ds {

template <typename Tp>
inline constexpr bool is_modint_v = false;

template <typename Tp>
concept modint = is_modint_v<std::remove_cvref_t<Tp>>;

} // namespace ds
} // namespace cp
#endif