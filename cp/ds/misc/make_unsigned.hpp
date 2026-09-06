#include <type_traits>

#ifndef CP_DS_MAKE_UNSIGNED
#define CP_DS_MAKE_UNSIGNED
namespace cp {
namespace ds {

template <typename Tp>
struct make_unsigned {
    using type = std::make_unsigned_t<Tp>;
};

#ifdef __SIZEOF_INT128__
template <>
struct make_unsigned<__int128> {
    using type = unsigned __int128; // Tự gắp int128 cho sạch sẽ
};

template <>
struct make_unsigned<unsigned __int128> {
    using type = unsigned __int128;
};
#endif

// Bí danh ngắn kiểu C++
template <typename Tp>
using make_unsigned_t = typename make_unsigned<Tp>::type;


} // namespace ds
} // namespace cp
#endif