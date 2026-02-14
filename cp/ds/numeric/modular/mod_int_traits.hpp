#include <type_traits>

#ifndef CP_DS_MODULAR_MOD_INT_TRAITS
#define CP_DS_MODULAR_MOD_INT_TRAITS
namespace cp {
namespace internal {

template <unsigned long long MOD>
class mod_int_traits {
private:
    static constexpr bool is_small = (MOD <= 4294967295ULL);

public:
    using value_type = typename std::conditional<is_small, unsigned int, unsigned long long>::type;
    using double_type = typename std::conditional<is_small, unsigned long long, __uint128_t>::type;
};

} // namespace internal
} // namespace cp
#endif