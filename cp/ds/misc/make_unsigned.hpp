#include <type_traits>
#include "../numeric/types/int128.hpp"

#ifndef CP_DS_MAKE_UNSIGNED
#define CP_DS_MAKE_UNSIGNED
namespace cp {
namespace ds {

template <typename Tp>
struct make_unsigned {
    using type = typename std::make_unsigned<Tp>::type;
};

template <> 
struct make_unsigned<int128> { 
    using type = uint128;
};

template <>
struct make_unsigned<uint128> {
    using type = uint128;
};

} // namespace ds
} // namespace cp
#endif