#include <tuple>
#include "../misc/formatter.hpp"

#ifndef CP_DS_RECORD
#define CP_DS_RECORD
namespace cp {

namespace ds {

template <typename... Ts>
class record : public std::tuple<Ts...> {
public:
    using std::tuple<Ts...>::tuple;
    
    template <std::size_t I>
    [[nodiscard]] constexpr decltype(auto) get() & noexcept {
        return std::get<I>(*this);
    }

    template <std::size_t I>
    [[nodiscard]] constexpr decltype(auto) get() const & noexcept {
        return std::get<I>(*this);
    }

    template <std::size_t I>
    [[nodiscard]] constexpr decltype(auto) get() && noexcept {
        return std::get<I>(std::move(*this));
    }

    template <std::size_t I>
    [[nodiscard]] constexpr decltype(auto) get() const && noexcept {
        return std::get<I>(std::move(*this));
    }

    template <typename T>
    [[nodiscard]] constexpr decltype(auto) get() & noexcept {
        return std::get<T>(*this);
    }

    template <typename T>
    [[nodiscard]] constexpr decltype(auto) get() const & noexcept {
        return std::get<T>(*this);
    }

    template <typename T>
    [[nodiscard]] constexpr decltype(auto) get() && noexcept {
        return std::get<T>(std::move(*this));
    }

    template <typename T>
    [[nodiscard]] constexpr decltype(auto) get() const && noexcept {
        return std::get<T>(std::move(*this));
    }

    [[nodiscard]] constexpr decltype(auto) x(this auto&& self) noexcept 
        requires (sizeof...(Ts) > 0) {
        return std::get<0>(std::forward<decltype(self)>(self));
    }

    [[nodiscard]] constexpr decltype(auto) y(this auto&& self) noexcept 
        requires (sizeof...(Ts) > 1) {
        return std::get<1>(std::forward<decltype(self)>(self));
    }

    [[nodiscard]] constexpr decltype(auto) z(this auto&& self) noexcept 
        requires (sizeof...(Ts) > 2) {
        return std::get<2>(std::forward<decltype(self)>(self));
    }

    [[nodiscard]] constexpr decltype(auto) w(this auto&& self) noexcept 
        requires (sizeof...(Ts) > 3) {
        return std::get<3>(std::forward<decltype(self)>(self));
    }
};

template <typename... Ts>
record(Ts...) -> record<Ts...>;

} // namespace ds
//<
namespace internal {

template <typename... Args>
struct formatter<cp::ds::record<Args...>> {
    static void print(std::ostream& os, const cp::ds::record<Args...>& r) {
        os << style::color_green << "record" << style::reset;
        
        formatter<std::tuple<Args...>>::print(os, static_cast<const std::tuple<Args...>&>(r));
    }
};

} // namespace internal
//>
} // namespace cp

namespace std {
    
template <typename... Ts>
struct tuple_size<cp::ds::record<Ts...>> : std::tuple_size<std::tuple<Ts...>> {};

template <std::size_t I, typename... Ts>
struct tuple_element<I, cp::ds::record<Ts...>> : std::tuple_element<I, std::tuple<Ts...>> {};

}
#endif