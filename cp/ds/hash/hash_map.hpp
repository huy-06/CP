#include <utility>
#include <iterator>
#include <concepts>
#include "custom_hash.hpp"
#include "../misc/formatter.hpp"

#ifndef CP_DS_HASH_HASH_MAP
#define CP_DS_HASH_HASH_MAP
namespace cp {
namespace ds {

template <typename Key, typename Val>
class hash_map {
private:
    struct node {
        std::pair<Key, Val> kv;
        bool occ{false};
    };

public:
    using size_type = std::size_t;

    template <typename PairType, typename MapPtr>
    class iterator_base {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = PairType;
        using difference_type = std::ptrdiff_t;
        using pointer = PairType*;
        using reference = PairType&;

        iterator_base() : i(0), mp(nullptr) {}

        iterator_base(size_type _i, MapPtr _mp) : i(_i), mp(_mp) {}

        template <typename OPair, typename OMapPtr>
            requires std::convertible_to<OMapPtr, MapPtr>
        iterator_base(const iterator_base<OPair, OMapPtr>& o) : i(o.i), mp(o.mp) {}

        reference operator*() const {
            return mp->a[i].kv;
        }

        pointer operator->() const {
            return &(mp->a[i].kv);
        }

        iterator_base& operator++() {
            ++i;
            while (i < mp->cap && !mp->a[i].occ) ++i;
            return *this;
        }

        iterator_base operator++(int) {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator_base& operator--() {
            --i;
            while (i < mp->cap && !mp->a[i].occ) --i;
            return *this;
        }

        iterator_base operator--(int) {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        template <typename OPair, typename OMapPtr>
        bool operator==(const iterator_base<OPair, OMapPtr>& o) const {
            return i == o.i;
        }

        template <typename OPair, typename OMapPtr>
        bool operator!=(const iterator_base<OPair, OMapPtr>& o) const {
            return i != o.i;
        }

    private:
        size_type i;
        MapPtr mp;
        template <typename, typename> friend class iterator_base;
    };

    using iterator = iterator_base<std::pair<Key, Val>, hash_map*>;
    using const_iterator = iterator_base<const std::pair<Key, Val>, const hash_map*>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    hash_map(size_type n = 0) : siz(0), cap(0), msk(0) {
        size_type k = 8;
        while (k < (n << 1)) k <<= 1;
        build(k);
    }

    hash_map(const hash_map& o) 
        : siz(o.siz), cap(o.cap), msk(o.msk) {
        if (cap > 0) {
            a = std::make_unique<node[]>(cap);
            for (size_type i = 0; i < cap; ++i) a[i] = o.a[i];
        }
    }

    hash_map(hash_map&& o) noexcept
        : siz(o.siz), cap(o.cap), msk(o.msk), a(std::move(o.a)), h(o.h) {
        o.siz = o.cap = o.msk = 0;
    }

    hash_map& operator=(const hash_map& o) {
        if (this != &o) {
            siz = o.siz;
            cap = o.cap;
            msk = o.msk;

            if (cap > 0) {
                a = std::make_unique<node[]>(cap);
                for (size_type i = 0; i < cap; ++i) 
                    a[i] = o.a[i];
            } else {
                a.reset();
            }
        }
        return *this;
    }

    hash_map& operator=(hash_map&& o) noexcept {
        if (this != &o) {
            siz = o.siz;
            cap = o.cap;
            msk = o.msk;
            a = std::move(o.a); h = o.h;
            o.siz = o.cap = o.msk = 0;
        }
        return *this;
    }

    iterator begin() {
        size_type i = 0;
        while (i < cap && !a[i].occ) ++i;
        return iterator(i, this);
    }

    iterator end() {
        return iterator(cap, this);
    }

    const_iterator begin() const {
        return cbegin();
    }

    const_iterator end() const {
        return cend();
    }

    const_iterator cbegin() const {
        size_type i = 0;
        while (i < cap && !a[i].occ) ++i;
        return const_iterator(i, this);
    }

    const_iterator cend() const {
        return const_iterator(cap, this);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    size_type size() const {
        return siz;
    }

    size_type bucket_count() const {
        return cap;
    }

    bool empty() const {
        return siz == 0;
    }

    void clear() {
        if (siz > 0) {
            for (size_type i = 0; i < cap; ++i) {
                if (a[i].occ) {
                    a[i].occ = false;
                    a[i].kv = {};
                }
            }
            siz = 0;
        }
    }

    Val& operator[](const Key& k) {
        if ((siz << 1) >= cap) rehash(cap << 1);

        size_type i = find_pos(k);
        if (!a[i].occ) {
            a[i].kv.first = k;
            a[i].kv.second = Val{};
            a[i].occ = true;
            ++siz;
        }
        return a[i].kv.second;
    }

    iterator find(const Key& k) {
        if (!cap) return end();
        size_type i = find_pos(k);
        return a[i].occ ? iterator(i, this) : end();
    }

    const_iterator find(const Key& k) const {
        if (!cap) return cend();
        size_type i = find_pos(k);
        return a[i].occ ? const_iterator(i, this) : cend();
    }

    bool count(const Key& k) const {
        if (!cap) return false;
        return a[find_pos(k)].occ;
    }

    bool erase(const Key& k) {
        if (!cap) return false;
        size_type i = find_pos(k);
        if (!a[i].occ) return false;

        a[i].occ = false;
        a[i].kv = {};
        --siz;

        for (size_type j = i;;) {
            j = (j + 1) & msk;
            if (!a[j].occ) break;

            size_type hk = h(a[j].kv.first) & msk;
            bool in_cyc = (i < j) ? (i < hk && hk <= j) : (i < hk || hk <= j);

            if (!in_cyc) {
                a[i].kv = std::move(a[j].kv);
                a[i].occ = true;

                a[j].occ = false;
                a[j].kv = {};
                i = j;
            }
        }
        return true;
    }

private:
    size_type siz;
    size_type cap;
    size_type msk;
    internal::custom_hash h;
    std::unique_ptr<node[]> a;

    void build(size_type c) {
        cap = c;
        msk = c - 1;
        a = std::make_unique<node[]>(c);
    }

    inline size_type find_pos(const Key& k) const {
        size_type i = h(k) & msk;
        while (a[i].occ && !(a[i].kv.first == k))
            i = (i + 1) & msk;
        return i;
    }

    void rehash(size_type ncap) {
        size_type old_cap = cap;
        auto old_t = std::move(a);
        build(ncap);

        siz = 0;
        for (size_type i = 0; i < old_cap; ++i) {
            if (old_t[i].occ) {
                size_type j = h(old_t[i].kv.first) & msk;
                while (a[j].occ) j = (j + 1) & msk;

                a[j].kv = std::move(old_t[i].kv);
                a[j].occ = true;
                ++siz;
            }
        }
    }
};

} // namespace ds
//<
namespace internal {

template <typename... Args>
struct formatter<ds::hash_map<Args...>> {
    static void print(std::ostream& os, const ds::hash_map<Args...>& v) {
        os << style::color_green << "hash_map" << style::reset;
        open_bracket(os, "(");
        open_bracket(os, "[");
        bool first = true;
        for (const auto& x : v) {
            if (!first) os << ", ";
            first = false;
            print_item(os, x.first);
            os << ": ";
            print_item(os, x.second);
        }
        close_bracket(os, "]");
        close_bracket(os, ")");
    }
};

} // namespace internal
//>
} // namespace cp
#endif