template <class T>
using is_signed_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value 
                                || std::is_same<T, __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int128 =
    typename std::conditional<std::is_same<T, __uint128_t>::value
                                || std::is_same<T, unsigned __int128>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using make_unsigned_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value,
                              __uint128_t,
                              unsigned __int128>;

template <class T>
using is_integral = 
    typename std::conditional<std::is_integral<T>::value
                                || is_signed_int128<T>::value
                                || is_unsigned_int128<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_signed_int =
    typename std::conditional<(is_integral<T>::value && std::is_signed<T>::value)
                                || is_signed_int128<T>::value,
                              std::true_type,
                              std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<(is_integral<T>::value && std::is_unsigned<T>::value)
                                || is_unsigned_int128<T>::value,
                              std::true_type,
                              std::false_type>::type;


template <typename Tp>
using enable_if_signed_int = 
    typename std::enable_if<is_signed_int<Tp>::value>::type;

template <typename Tp>
using enable_if_unsigned_int = 
    typename std::enable_if<is_unsigned_int<Tp>::value>::type;