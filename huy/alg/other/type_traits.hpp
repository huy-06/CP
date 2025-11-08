#include <type_traits>

#ifndef HUY_ALG_TYPE_TRAITS
#define HUY_ALG_TYPE_TRAITS
namespace huy {

template <bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template <typename Tp>
struct is_signed_ext : std::integral_constant<bool, (Tp(-1) < Tp(0))> {};

template <typename Tp>
constexpr bool is_signed_v() {
    return is_signed_ext<Tp>::value;
}

template <typename Tp>
struct is_integral_helper : std::false_type {};

template <> struct is_integral_helper<bool> : std::true_type {};
template <> struct is_integral_helper<char> : std::true_type {};
template <> struct is_integral_helper<signed char> : std::true_type {};
template <> struct is_integral_helper<unsigned char> : std::true_type {};
template <> struct is_integral_helper<wchar_t> : std::true_type {};
template <> struct is_integral_helper<char16_t> : std::true_type {};
template <> struct is_integral_helper<char32_t> : std::true_type {};
template <> struct is_integral_helper<short> : std::true_type {};
template <> struct is_integral_helper<unsigned short> : std::true_type {};
template <> struct is_integral_helper<int> : std::true_type {};
template <> struct is_integral_helper<unsigned int> : std::true_type {};
template <> struct is_integral_helper<long> : std::true_type {};
template <> struct is_integral_helper<unsigned long> : std::true_type {};
template <> struct is_integral_helper<long long> : std::true_type {};
template <> struct is_integral_helper<unsigned long long> : std::true_type {};

#if defined(__SIZEOF_INT128__)
template <> struct is_integral_helper<__int128> : std::true_type {};
template <> struct is_integral_helper<unsigned __int128> : std::true_type {};
#endif

template <typename Tp>
struct is_integral_ext : is_integral_helper< typename std::remove_cv<Tp>::type > {};

template <typename Tp>
constexpr bool is_integral_v() {
    return is_integral_ext<Tp>::value;
}

template <typename Tp>   
struct make_unsigned_ext;

template <> struct make_unsigned_ext<signed char>         { using type = unsigned char; };
template <> struct make_unsigned_ext<short>               { using type = unsigned short; };
template <> struct make_unsigned_ext<int>                 { using type = unsigned int; };
template <> struct make_unsigned_ext<long>                { using type = unsigned long; };
template <> struct make_unsigned_ext<long long>           { using type = unsigned long long; };

#if defined(__SIZEOF_INT128__)
template <> struct make_unsigned_ext<__int128>            { using type = unsigned __int128; };
template <> struct make_unsigned_ext<unsigned __int128>   { using type = unsigned __int128; };
#endif

template <> struct make_unsigned_ext<unsigned char>       { using type = unsigned char; };
template <> struct make_unsigned_ext<unsigned short>      { using type = unsigned short; };
template <> struct make_unsigned_ext<unsigned int>        { using type = unsigned int; };
template <> struct make_unsigned_ext<unsigned long>       { using type = unsigned long; };
template <> struct make_unsigned_ext<unsigned long long>  { using type = unsigned long long; };

template <typename Tp>
using make_unsigned_t = typename make_unsigned_ext<Tp>::type;

} // namespace huy
#endif