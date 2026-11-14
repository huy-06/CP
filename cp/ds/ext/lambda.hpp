#include <functional>
#include <type_traits>
#include <utility>

#ifndef CP_DS_LAMBDA
#define CP_DS_LAMBDA
namespace cp {

namespace internal {

template <class Fun>
class lambda {
    Fun fn;

public:
    template <class T, std::enable_if_t<!std::is_same<std::decay_t<T>, lambda>::value, int> = 0>
    explicit lambda(T&& f) noexcept(std::is_nothrow_constructible<Fun, T&&>::value)
        : fn(std::forward<T>(f)) {}

    template <class... Args>
    decltype(auto) operator()(Args&&... args) {
        return fn(std::ref(*this), std::forward<Args>(args)...);
    }

    template <class... Args>
    decltype(auto) operator()(Args&&... args) const {
        return fn(std::ref(*this), std::forward<Args>(args)...);
    }
};

} // namespace interal

namespace ds {

template <class Fun>
internal::lambda<std::decay_t<Fun>> lambda(Fun&& f) {
    return internal::lambda<std::decay_t<Fun>>(std::forward<Fun>(f));
}

} // namespace ds

} // namespace cp
#endif