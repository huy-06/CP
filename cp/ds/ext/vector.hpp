#include <algorithm>
#include <bit>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "../ext/formatter.hpp"

#ifndef CP_DS_VECTOR
#define CP_DS_VECTOR

#if defined(__GNUC__) || defined(__clang__)
    #define CP_NOINLINE [[gnu::noinline]]
#else
    #define CP_NOINLINE
#endif

namespace cp {
namespace ds {

namespace internal {

struct slab_pool {
    static constexpr std::size_t min_shift   = 4;  // 16 bytes
    static constexpr std::size_t max_shift   = 12; // 4096 bytes
    static constexpr std::size_t min_bytes   = std::size_t{1} << min_shift;
    static constexpr std::size_t max_bytes   = std::size_t{1} << max_shift;
    static constexpr std::size_t num_classes = max_shift - min_shift + 1; // 9 classes

    struct alignas(16) slab_header {
        slab_header* next;
    };

    struct pool_state {
        void* free_lists[num_classes] = {};
        slab_header* slabs            = nullptr;
        char* bump_cur                = nullptr;
        char* bump_end                = nullptr;
        std::size_t next_slab_bytes   = 64 * 1024;
        std::size_t live_allocs       = 0;
        bool exiting                  = false;
    };

    struct pool_cleaner {
        ~pool_cleaner() noexcept {
            state.exiting = true;
            if (state.live_allocs == 0) {
                release_all();
            }
        }
    };

    static pool_state state;
    static pool_cleaner cleaner;

    struct alloc_result {
        void* ptr;
        std::size_t actual_bytes;
    };

    static void release_all() noexcept {
        slab_header* cur = state.slabs;
        while (cur != nullptr) {
            slab_header* nxt = cur->next;
            std::free(cur);
            cur = nxt;
        }
        state.slabs           = nullptr;
        state.bump_cur        = nullptr;
        state.bump_end        = nullptr;
        state.next_slab_bytes = 64 * 1024;
        for (void*& head : state.free_lists) {
            head = nullptr;
        }
    }

    [[nodiscard]] static constexpr std::size_t class_index(std::size_t bytes) noexcept {
        if (bytes <= min_bytes) return 0;
        return static_cast<std::size_t>(std::bit_width(bytes - 1)) - min_shift;
    }

    [[nodiscard]] static alloc_result allocate(std::size_t bytes) {
        const std::size_t cls = class_index(bytes);
        const std::size_t bsz = std::size_t{1} << (cls + min_shift);

        if (void* p = state.free_lists[cls]; p != nullptr) [[likely]] {
            std::memcpy(&state.free_lists[cls], p, sizeof(void*));
            ++state.live_allocs;
            return {p, bsz};
        }

        if (state.bump_cur == nullptr ||
            static_cast<std::size_t>(state.bump_end - state.bump_cur) < bsz) [[unlikely]] {
            refill_slab(bsz);
        }

        void* p = state.bump_cur;
        state.bump_cur += bsz;
        ++state.live_allocs;
        return {p, bsz};
    }

    static void deallocate(void* p, std::size_t bytes) noexcept {
        const std::size_t cls = class_index(bytes);
        std::memcpy(p, &state.free_lists[cls], sizeof(void*));
        state.free_lists[cls] = p;
        --state.live_allocs;
        if (state.exiting && state.live_allocs == 0) [[unlikely]] {
            release_all();
        }
    }

private:
    CP_NOINLINE static void refill_slab(std::size_t min_bsz) {
        if (state.bump_cur != nullptr) {
            std::size_t rem = static_cast<std::size_t>(state.bump_end - state.bump_cur);
            while (rem >= min_bytes) {
                std::size_t chunk_sz = std::min(std::bit_floor(rem), max_bytes);
                std::size_t rem_cls  = class_index(chunk_sz);
                std::memcpy(state.bump_cur, &state.free_lists[rem_cls], sizeof(void*));
                state.free_lists[rem_cls] = state.bump_cur;
                state.bump_cur += chunk_sz;
                rem -= chunk_sz;
            }
        }

        const std::size_t slab_data_bytes = std::max(state.next_slab_bytes, min_bsz);
        void* raw = std::malloc(sizeof(slab_header) + slab_data_bytes);
        if (!raw) [[unlikely]] {
            throw std::bad_alloc();
        }

        auto* hdr      = static_cast<slab_header*>(raw);
        hdr->next      = state.slabs;
        state.slabs    = hdr;
        state.bump_cur = static_cast<char*>(raw) + sizeof(slab_header);
        state.bump_end = state.bump_cur + slab_data_bytes;

        if (state.next_slab_bytes < (std::size_t{1} << 20)) {
            state.next_slab_bytes *= 2;
        }
    }
};

inline slab_pool::pool_state slab_pool::state{};
inline slab_pool::pool_cleaner slab_pool::cleaner{};

} // namespace internal

template <typename T>
struct is_trivially_relocatable : std::is_trivially_copyable<T> {};

template <typename T>
inline constexpr bool is_trivially_relocatable_v = is_trivially_relocatable<T>::value;

template <typename T, std::size_t InlineCap = 0>
class vector;

template <typename T, std::size_t InlineCap>
struct is_trivially_relocatable<vector<T, InlineCap>> : is_trivially_relocatable<T> {};

template <typename T, std::size_t InlineCap>
class vector {
    static_assert(!std::is_reference_v<T>, "vector cannot hold references");

public:
    using value_type             = T;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    static constexpr bool use_pool_v =
        is_trivially_relocatable_v<T> && (alignof(T) <= 16) && (alignof(T) <= alignof(std::max_align_t));

    struct heap_storage {
        T* heap = nullptr;
    };

    union sbo_storage {
        T* heap;
        alignas(T) std::byte sbo[sizeof(T) * (InlineCap > 0 ? InlineCap : 1)];
        sbo_storage() noexcept {}
        ~sbo_storage() noexcept {}
    };

    using storage = std::conditional_t<(InlineCap > 0), sbo_storage, heap_storage>;

    storage _storage;
    std::uint32_t _siz = 0;
    std::uint32_t _cap = static_cast<std::uint32_t>(InlineCap);

    [[nodiscard]] constexpr bool is_inline() const noexcept {
        if constexpr (InlineCap == 0) return false;
        else return _cap <= InlineCap;
    }

    [[nodiscard]] T* raw_sbo() noexcept {
        return reinterpret_cast<T*>(_storage.sbo);
    }

    [[nodiscard]] const T* raw_sbo() const noexcept {
        return reinterpret_cast<const T*>(_storage.sbo);
    }

    [[nodiscard]] static T* alloc_buf(size_type min_cap, size_type& out_cap) {
        if (min_cap == 0) {
            out_cap = 0;
            return nullptr;
        }
        if constexpr (use_pool_v) {
            const size_type bytes = min_cap * sizeof(T);
            if (bytes <= internal::slab_pool::max_bytes) {
                auto [p, actual_bytes] = internal::slab_pool::allocate(bytes);
                out_cap = actual_bytes / sizeof(T);
                return static_cast<T*>(p);
            }
            void* p = std::malloc(bytes);
            if (!p) [[unlikely]] throw std::bad_alloc();
            out_cap = min_cap;
            return static_cast<T*>(p);
        } else {
            out_cap = min_cap;
            return static_cast<T*>(::operator new(min_cap * sizeof(T), std::align_val_t{alignof(T)}));
        }
    }

    static void free_buf(T* p, size_type cap) noexcept {
        if (!p) return;
        if constexpr (use_pool_v) {
            const size_type bytes = cap * sizeof(T);
            if (bytes <= internal::slab_pool::max_bytes) {
                internal::slab_pool::deallocate(p, bytes);
            } else {
                std::free(p);
            }
        } else {
            ::operator delete(p, std::align_val_t{alignof(T)});
        }
    }

    static void relocate_n(T* src, size_type n, T* dst) {
        if (n == 0) return;
        if constexpr (is_trivially_relocatable_v<T>) {
            std::memcpy(static_cast<void*>(dst), static_cast<const void*>(src), n * sizeof(T));
        } else {
            std::uninitialized_move_n(src, n, dst);
            std::destroy_n(src, n);
        }
    }

    [[nodiscard]] size_type next_cap(size_type min_cap) const {
        if (min_cap > max_size()) [[unlikely]] {
            throw std::length_error("vector: capacity overflow");
        }
        size_type doubled = static_cast<size_type>(_cap) * 2;
        if constexpr (InlineCap == 0) {
            constexpr size_type min_elems = std::max<size_type>(4, 16 / sizeof(T));
            if (doubled < min_elems) doubled = min_elems;
        }
        if (doubled > max_size()) doubled = max_size();
        return std::max(doubled, min_cap);
    }

    void reallocate(size_type min_new_cap) {
        if (min_new_cap <= _cap) return;
        if (min_new_cap > max_size()) [[unlikely]] {
            throw std::length_error("vector: capacity overflow");
        }

        if constexpr (InlineCap > 0) {
            if (is_inline()) {
                size_type actual_cap = 0;
                T* new_ptr = alloc_buf(min_new_cap, actual_cap);
                try {
                    relocate_n(raw_sbo(), _siz, new_ptr);
                } catch (...) {
                    free_buf(new_ptr, actual_cap);
                    throw;
                }
                _storage.heap = new_ptr;
                _cap = static_cast<std::uint32_t>(actual_cap);
                return;
            }
        }

        if constexpr (use_pool_v) {
            const size_type old_bytes = static_cast<size_type>(_cap) * sizeof(T);
            const size_type new_bytes = min_new_cap * sizeof(T);
            if (_storage.heap != nullptr &&
                old_bytes > internal::slab_pool::max_bytes &&
                new_bytes > internal::slab_pool::max_bytes) {
                void* p = std::realloc(_storage.heap, new_bytes);
                if (!p) [[unlikely]] throw std::bad_alloc();
                _storage.heap = static_cast<T*>(p);
                _cap = static_cast<std::uint32_t>(min_new_cap);
                return;
            }
        }

        size_type actual_cap = 0;
        T* new_ptr = alloc_buf(min_new_cap, actual_cap);
        try {
            relocate_n(_storage.heap, _siz, new_ptr);
        } catch (...) {
            free_buf(new_ptr, actual_cap);
            throw;
        }
        free_buf(_storage.heap, _cap);
        _storage.heap = new_ptr;
        _cap = static_cast<std::uint32_t>(actual_cap);
    }

    void destroy_elements() noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            std::destroy_n(data(), _siz);
        }
        _siz = 0;
    }

    template <typename... Args>
    CP_NOINLINE reference emplace_back_slow(Args&&... args) {
        T tmp(std::forward<Args>(args)...);
        reallocate(next_cap(static_cast<size_type>(_siz) + 1));
        T* p = std::construct_at(data() + _siz, std::move(tmp));
        ++_siz;
        return *p;
    }

public:
    vector() noexcept = default;

    explicit vector(size_type count) {
        resize(count);
    }

    vector(size_type count, const T& value) {
        assign(count, value);
    }

    template <std::input_iterator InputIt>
    vector(InputIt first, InputIt last) {
        assign(first, last);
    }

    vector(std::initializer_list<T> init) {
        assign(init.begin(), init.end());
    }

    vector(const vector& rhs) {
        if (rhs._siz == 0) return;
        if (rhs._siz > InlineCap) {
            size_type actual_cap = 0;
            _storage.heap = alloc_buf(rhs._siz, actual_cap);
            _cap = static_cast<std::uint32_t>(actual_cap);
        }
        try {
            std::uninitialized_copy_n(rhs.data(), rhs._siz, data());
        } catch (...) {
            if (!is_inline()) {
                free_buf(_storage.heap, _cap);
                _cap = static_cast<std::uint32_t>(InlineCap);
                if constexpr (InlineCap == 0) _storage.heap = nullptr;
            }
            throw;
        }
        _siz = rhs._siz;
    }

    vector(vector&& rhs) noexcept(InlineCap == 0 || std::is_nothrow_move_constructible_v<T>) {
        if (!rhs.is_inline()) {
            _storage.heap = rhs._storage.heap;
            _siz = rhs._siz;
            _cap = rhs._cap;
            rhs._siz = 0;
            rhs._cap = static_cast<std::uint32_t>(InlineCap);
            if constexpr (InlineCap == 0) rhs._storage.heap = nullptr;
        } else {
            if constexpr (InlineCap > 0) {
                relocate_n(rhs.raw_sbo(), rhs._siz, raw_sbo());
                _siz = rhs._siz;
                rhs._siz = 0;
            }
        }
    }

    ~vector() noexcept {
        destroy_elements();
        if (!is_inline()) {
            free_buf(_storage.heap, _cap);
        }
    }

    vector& operator=(const vector& rhs) {
        if (this == &rhs) return *this;
        if (rhs._siz > _cap) {
            clear();
            reserve(rhs._siz);
            std::uninitialized_copy_n(rhs.data(), rhs._siz, data());
            _siz = rhs._siz;
        } else {
            const size_type common = std::min<size_type>(_siz, rhs._siz);
            std::copy_n(rhs.data(), common, data());
            if (rhs._siz > _siz) {
                std::uninitialized_copy_n(rhs.data() + _siz, rhs._siz - _siz, data() + _siz);
            } else if (rhs._siz < _siz) {
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    std::destroy(data() + rhs._siz, data() + _siz);
                }
            }
            _siz = rhs._siz;
        }
        return *this;
    }

    vector& operator=(vector&& rhs) noexcept(InlineCap == 0 || std::is_nothrow_move_constructible_v<T>) {
        if (this == &rhs) return *this;
        destroy_elements();
        if (!is_inline()) {
            free_buf(_storage.heap, _cap);
            _cap = static_cast<std::uint32_t>(InlineCap);
            if constexpr (InlineCap == 0) _storage.heap = nullptr;
        }

        if (!rhs.is_inline()) {
            _storage.heap = rhs._storage.heap;
            _siz = rhs._siz;
            _cap = rhs._cap;
            rhs._siz = 0;
            rhs._cap = static_cast<std::uint32_t>(InlineCap);
            if constexpr (InlineCap == 0) rhs._storage.heap = nullptr;
        } else {
            if constexpr (InlineCap > 0) {
                relocate_n(rhs.raw_sbo(), rhs._siz, raw_sbo());
                _siz = rhs._siz;
                rhs._siz = 0;
            }
        }
        return *this;
    }

    vector& operator=(std::initializer_list<T> init) {
        assign(init.begin(), init.end());
        return *this;
    }

    [[nodiscard]] pointer data() noexcept {
        if constexpr (InlineCap == 0) return _storage.heap;
        else return is_inline() ? raw_sbo() : _storage.heap;
    }

    [[nodiscard]] const_pointer data() const noexcept {
        if constexpr (InlineCap == 0) return _storage.heap;
        else return is_inline() ? raw_sbo() : _storage.heap;
    }

    [[nodiscard]] reference operator[](size_type pos) noexcept {
        return data()[pos];
    }

    [[nodiscard]] const_reference operator[](size_type pos) const noexcept {
        return data()[pos];
    }

    [[nodiscard]] reference at(size_type pos) {
        if (pos >= _siz) [[unlikely]] throw std::out_of_range("vector::at");
        return data()[pos];
    }

    [[nodiscard]] const_reference at(size_type pos) const {
        if (pos >= _siz) [[unlikely]] throw std::out_of_range("vector::at");
        return data()[pos];
    }

    [[nodiscard]] reference front() noexcept {
        return data()[0];
    }

    [[nodiscard]] const_reference front() const noexcept {
        return data()[0];
    }

    [[nodiscard]] reference back() noexcept {
        return data()[_siz - 1];
    }

    [[nodiscard]] const_reference back() const noexcept {
        return data()[_siz - 1];
    }

    [[nodiscard]] iterator begin() noexcept {
        return data();
    }

    [[nodiscard]] const_iterator begin() const noexcept {
        return data();
    }

    [[nodiscard]] const_iterator cbegin() const noexcept {
        return data();
    }

    [[nodiscard]] iterator end() noexcept {
        return data() + _siz;
    }

    [[nodiscard]] const_iterator end() const noexcept {
        return data() + _siz;
    }

    [[nodiscard]] const_iterator cend() const noexcept {
        return data() + _siz;
    }

    [[nodiscard]] reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    [[nodiscard]] reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    [[nodiscard]] const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    [[nodiscard]] const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    [[nodiscard]] bool empty() const noexcept {
        return _siz == 0;
    }

    [[nodiscard]] size_type size() const noexcept {
        return _siz;
    }

    [[nodiscard]] size_type capacity() const noexcept {
        return _cap;
    }

    [[nodiscard]] constexpr size_type max_size() const noexcept {
        return std::min<size_type>(
            std::numeric_limits<std::uint32_t>::max(),
            static_cast<size_type>(std::numeric_limits<difference_type>::max()) / sizeof(T)
        );
    }

    void reserve(size_type new_cap) {
        if (new_cap > _cap) {
            reallocate(new_cap);
        }
    }

    void shrink_to_fit() {
        if (is_inline()) return;
        if (_siz <= InlineCap) {
            T* old_heap = _storage.heap;
            const size_type old_cap = _cap;
            if constexpr (InlineCap > 0) {
                relocate_n(old_heap, _siz, raw_sbo());
            }
            free_buf(old_heap, old_cap);
            _cap = static_cast<std::uint32_t>(InlineCap);
            if constexpr (InlineCap == 0) _storage.heap = nullptr;
        } else if (_siz < _cap) {
            if constexpr (use_pool_v) {
                const size_type cur_bytes = static_cast<size_type>(_cap) * sizeof(T);
                const size_type req_bytes = static_cast<size_type>(_siz) * sizeof(T);
                if (cur_bytes <= internal::slab_pool::max_bytes &&
                    internal::slab_pool::class_index(req_bytes) == internal::slab_pool::class_index(cur_bytes)) {
                    return;
                }
            }
            size_type actual_cap = 0;
            T* new_ptr = alloc_buf(_siz, actual_cap);
            try {
                relocate_n(_storage.heap, _siz, new_ptr);
            } catch (...) {
                free_buf(new_ptr, actual_cap);
                throw;
            }
            free_buf(_storage.heap, _cap);
            _storage.heap = new_ptr;
            _cap = static_cast<std::uint32_t>(actual_cap);
        }
    }

    void clear() noexcept {
        destroy_elements();
    }

    template <typename... Args>
    reference emplace_back(Args&&... args) {
        if (_siz == _cap) [[unlikely]] {
            return emplace_back_slow(std::forward<Args>(args)...);
        }
        T* p = std::construct_at(data() + _siz, std::forward<Args>(args)...);
        ++_siz;
        return *p;
    }

    void push_back(const T& value) {
        emplace_back(value);
    }

    void push_back(T&& value) {
        emplace_back(std::move(value));
    }

    void pop_back() noexcept {
        --_siz;
        if constexpr (!std::is_trivially_destructible_v<T>) {
            std::destroy_at(data() + _siz);
        }
    }

    void resize(size_type count) {
        if (count < _siz) {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                std::destroy(data() + count, data() + _siz);
            }
            _siz = static_cast<std::uint32_t>(count);
        } else if (count > _siz) {
            if (count > _cap) reallocate(next_cap(count));
            std::uninitialized_value_construct_n(data() + _siz, count - _siz);
            _siz = static_cast<std::uint32_t>(count);
        }
    }

    void resize(size_type count, const T& value) {
        if (count < _siz) {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                std::destroy(data() + count, data() + _siz);
            }
            _siz = static_cast<std::uint32_t>(count);
        } else if (count > _siz) {
            if (count > _cap) {
                T val_copy = value;
                reallocate(next_cap(count));
                std::uninitialized_fill_n(data() + _siz, count - _siz, val_copy);
            } else {
                std::uninitialized_fill_n(data() + _siz, count - _siz, value);
            }
            _siz = static_cast<std::uint32_t>(count);
        }
    }

    void resize_uninitialized(size_type count)
        requires (std::is_trivially_default_constructible_v<T> && std::is_trivially_destructible_v<T>)
    {
        if (count > _cap) reallocate(next_cap(count));
        _siz = static_cast<std::uint32_t>(count);
    }

    void assign(size_type count, const T& value) {
        T val_copy = value;
        clear();
        reserve(count);
        std::uninitialized_fill_n(data(), count, val_copy);
        _siz = static_cast<std::uint32_t>(count);
    }

    template <std::input_iterator InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        if constexpr (std::forward_iterator<InputIt>) {
            auto count = static_cast<size_type>(std::distance(first, last));
            reserve(count);
            std::uninitialized_copy(first, last, data());
            _siz = static_cast<std::uint32_t>(count);
        } else {
            for (; first != last; ++first) {
                emplace_back(*first);
            }
        }
    }

    void assign(std::initializer_list<T> init) {
        assign(init.begin(), init.end());
    }

    iterator insert(const_iterator pos, const T& value) {
        return emplace(pos, value);
    }

    iterator insert(const_iterator pos, T&& value) {
        return emplace(pos, std::move(value));
    }

    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        size_type idx = static_cast<size_type>(pos - cbegin());
        if (idx == _siz) {
            emplace_back(std::forward<Args>(args)...);
            return begin() + idx;
        }
        T tmp(std::forward<Args>(args)...);
        if (_siz == _cap) [[unlikely]] {
            reallocate(next_cap(static_cast<size_type>(_siz) + 1));
        }
        T* p = data();
        std::construct_at(p + _siz, std::move(p[_siz - 1]));
        std::move_backward(p + idx, p + _siz - 1, p + _siz);
        p[idx] = std::move(tmp);
        ++_siz;
        return p + idx;
    }

    iterator erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }

    iterator erase(const_iterator first, const_iterator last) {
        size_type idx = static_cast<size_type>(first - cbegin());
        size_type cnt = static_cast<size_type>(last - first);
        if (cnt == 0) return begin() + idx;

        T* p = data();
        std::move(p + idx + cnt, p + _siz, p + idx);
        if constexpr (!std::is_trivially_destructible_v<T>) {
            std::destroy(p + _siz - cnt, p + _siz);
        }
        _siz -= static_cast<std::uint32_t>(cnt);
        return p + idx;
    }

    void swap(vector& rhs) noexcept(InlineCap == 0 || std::is_nothrow_swappable_v<T>) {
        if (this == &rhs) return;
        if (!is_inline() && !rhs.is_inline()) {
            std::swap(_storage.heap, rhs._storage.heap);
            std::swap(_siz, rhs._siz);
            std::swap(_cap, rhs._cap);
            return;
        }
        vector tmp(std::move(rhs));
        rhs = std::move(*this);
        *this = std::move(tmp);
    }

    friend void swap(vector& lhs, vector& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }

    friend bool operator==(const vector& lhs, const vector& rhs) {
        return lhs._siz == rhs._siz && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    friend auto operator<=>(const vector& lhs, const vector& rhs) {
        return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }
};

} // namespace ds

namespace alg {

template <typename T, std::size_t InlineCap>
void swap(ds::vector<T, InlineCap>& lhs, ds::vector<T, InlineCap>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace alg
//<
namespace internal {

template <typename... Args>
struct formatter<ds::vector<Args...>> {
    static void print(std::ostream& os, const ds::vector<Args...>& v) {
        open_bracket(os, "[");
        bool first = true;
        for (const auto& x : v) {
            if (!first) os << ", ";
            first = false;
            print_item(os, x);
        }
        close_bracket(os, "]");
    }
};

} // namespace internal
//>

} // namespace cp

#undef CP_NOINLINE
#endif