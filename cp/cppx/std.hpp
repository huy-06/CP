#include "write.hpp"

#ifndef CP_CPPX_STD
#define CP_CPPX_STD

template <typename Tp, typename std::enable_if<!std::is_convertible<typename std::decay<Tp>::type, std::string>::value, int>::type>
std::string& operator+=(std::string& lhs, const Tp& rhs) {
    lhs += cp::tostr(rhs);
    return lhs;
}

template <typename Tp, typename std::enable_if<!std::is_convertible<typename std::decay<Tp>::type, std::string>::value, int>::type>
std::string operator+(std::string lhs, const Tp& rhs) {
    lhs += rhs;
    return lhs;
}

template <typename Tp, typename std::enable_if<!std::is_convertible<typename std::decay<Tp>::type, std::string>::value, int>::type>
std::string operator+(const Tp& lhs, const std::string& rhs) {
    return cp::tostr(lhs) + rhs;
}

template <typename Tp>
std::string operator*=(std::string& lhs, const Tp& rhs) {
    const std::string t = lhs;
    for (Tp i = 0; i < rhs; ++i) {
        lhs += t;
    }
    return lhs;
}

template <typename Tp>
std::string operator*(std::string lhs, const Tp& rhs) {
    return lhs *= rhs;
}

template <typename Tp>
std::vector<Tp> operator+=(std::vector<Tp>& lhs, const Tp& rhs) {
    lhs.push_back(rhs);
    return lhs;
}

template <typename Tp>
std::vector<Tp> operator+=(std::vector<Tp>& lhs, const std::vector<Tp>& rhs) {
    for (const auto& i : rhs) {
        lhs.push_back(i);
    }
    return lhs;
}

template <typename Tp>
std::vector<Tp> operator+(std::vector<Tp> lhs, const Tp& rhs) {
    return lhs += rhs;
}

template <typename Tp>
std::vector<Tp> operator+(std::vector<Tp> lhs, const std::vector<Tp>& rhs) {
    return lhs += rhs;
}

template <typename Tp>
std::vector<Tp> operator+(const Tp& lhs, const std::vector<Tp>& rhs) {
    return std::vector<Tp>{lhs} + rhs;
}

template <typename T1, typename T2>
std::vector<T1> operator*=(std::vector<T1>& lhs, const T2& rhs) {
    const std::vector<T1> t = lhs;
    for (T2 i = 0; i < rhs; ++i) {
        lhs += t;
    }
    return lhs;
}

template <typename T1, typename T2>
std::vector<T1> operator*(std::vector<T1> lhs, const T2& rhs) {
    return lhs *= rhs;
}

#endif