#include <vector>
#include <array>
#include <stdexcept>
#include <format>
#include <cassert>
#include <concepts>
#include <compare>
#include <algorithm>

#ifndef CP_DS_TENSOR_ND_ARRAY
#define CP_DS_TENSOR_ND_ARRAY
namespace cp {
namespace ds {

template <typename Tp>
class nd_array {
public:
    using value_type      = Tp;
    using size_type       = std::size_t;
    using reference       = Tp&;
    using const_reference = const Tp&;
    using iterator        = typename std::vector<Tp>::iterator;
    using const_iterator  = typename std::vector<Tp>::const_iterator;

    template <typename... Dims>
    requires (std::convertible_to<Dims, size_type> && ...)
    explicit nd_array(Dims... dims) : m_dims{ static_cast<size_type>(dims)... } {
        recalculate_strides();

        size_type total_size = 1;
        for (auto d : m_dims) {
            total_size *= d;
        }
        m_data.resize(total_size, Tp{});
    }

    template <typename... Idxs>
    requires (std::convertible_to<Idxs, size_type> && ...)
    [[nodiscard]] constexpr reference operator[](Idxs... idxs) noexcept {
        assert(sizeof...(Idxs) == m_dims.size() && "Number of indices must match dimensions!");

        const std::array<size_type, sizeof...(Idxs)> indices{ static_cast<size_type>(idxs)... };
        size_type flat_idx = 0;

        for (size_type i = 0; i < sizeof...(Idxs); ++i) {
            flat_idx += indices[i] * m_strides[i];
        }

        return m_data[flat_idx];
    }

    template <typename... Idxs>
    requires (std::convertible_to<Idxs, size_type> && ...)
    [[nodiscard]] constexpr const_reference operator[](Idxs... idxs) const noexcept {
        assert(sizeof...(Idxs) == m_dims.size() && "Number of indices must match dimensions!");

        const std::array<size_type, sizeof...(Idxs)> indices{ static_cast<size_type>(idxs)... };
        size_type flat_idx = 0;

        for (size_type i = 0; i < sizeof...(Idxs); ++i) {
            flat_idx += indices[i] * m_strides[i];
        }

        return m_data[flat_idx];
    }

    template <typename... Idxs>
    requires (std::convertible_to<Idxs, size_type> && ...)
    [[nodiscard]] constexpr reference at(Idxs... idxs) {
        if (sizeof...(Idxs) != m_dims.size()) {
            throw std::invalid_argument(std::format(
                "Invalid argument: Array has {} dimensions, but accessed with {} indices.",
                m_dims.size(), sizeof...(Idxs)));
        }

        const std::array<size_type, sizeof...(Idxs)> indices{ static_cast<size_type>(idxs)... };
        size_type flat_idx = 0;

        for (size_type i = 0; i < sizeof...(Idxs); ++i) {
            if (indices[i] >= m_dims[i]) {
                throw std::out_of_range(std::format(
                    "Out of range at dimension {}: accessed index [{}], but maximum is [{}].",
                    i + 1, indices[i], m_dims[i] - 1));
            }
            flat_idx += indices[i] * m_strides[i];
        }

        return m_data[flat_idx];
    }

    [[nodiscard]] constexpr size_type dimensions() const noexcept {
        return m_dims.size();
    }

    [[nodiscard]] constexpr size_type size() const noexcept {
        return m_data.size();
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return m_data.empty();
    }

    [[nodiscard]] const std::vector<size_type>& shape() const noexcept {
        return m_dims;
    }

    [[nodiscard]] constexpr iterator begin() noexcept {
        return m_data.begin();
    }

    [[nodiscard]] constexpr iterator end() noexcept {
        return m_data.end();
    }

    [[nodiscard]] constexpr const_iterator begin() const noexcept {
        return m_data.begin();
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept {
        return m_data.end();
    }

    [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
        return m_data.cbegin();
    }

    [[nodiscard]] constexpr const_iterator cend() const noexcept {
        return m_data.cend();
    }

    constexpr void fill(const Tp& value) {
        std::fill(m_data.begin(), m_data.end(), value);
    }

    constexpr void clear() noexcept {
        m_data.clear();
        m_dims.clear();
        m_strides.clear();
    }

    template <typename... Dims>
    requires (std::convertible_to<Dims, size_type> && ...)
    constexpr void reshape(Dims... dims) {
        std::vector<size_type> new_dims{ static_cast<size_type>(dims)... };

        size_type new_total_size = 1;
        for (auto d : new_dims) {
            new_total_size *= d;
        }

        if (new_total_size != m_data.size()) {
            throw std::invalid_argument(std::format(
                "Reshape error: Cannot reshape array of size {} into shape with total size {}.",
                m_data.size(), new_total_size));
        }

        m_dims = std::move(new_dims);
        recalculate_strides();
    }

    [[nodiscard]] friend bool operator==(const nd_array& lhs, const nd_array& rhs) noexcept {
        return lhs.m_dims == rhs.m_dims && lhs.m_data == rhs.m_data;
    }

    [[nodiscard]] friend auto operator<=>(const nd_array& lhs, const nd_array& rhs) noexcept {
        if (auto cmp = lhs.m_dims <=> rhs.m_dims; cmp != 0) {
            return cmp;
        }
        return lhs.m_data <=> rhs.m_data;
    }

private:
    std::vector<Tp>        m_data;
    std::vector<size_type> m_dims;
    std::vector<size_type> m_strides;

    constexpr void recalculate_strides() {
        m_strides.resize(m_dims.size());
        if (!m_dims.empty()) {
            size_type current_stride = 1;
            for (ptrdiff_t i = m_dims.size() - 1; i >= 0; --i) {
                m_strides[i] = current_stride;
                current_stride *= m_dims[i];
            }
        }
    }
};

} // namespace ds
} // namespace cp
#endif