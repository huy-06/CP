#include <memory>
#include "custom_hash.hpp"

#ifndef CP_DS_HASH_FAST_HASH_MAP
#define CP_DS_HASH_FAST_HASH_MAP
namespace cp {
namespace ds {

template <typename Key, typename Val>
class fast_hash_map {
private:
    struct node {
        Key first;
        Val second;
        bool occupied;

        node() : occupied(false) {}
    };

public:
    using size_type = std::size_t;

    struct entry {
        const Key& first;
        Val& second;

        operator std::tuple<const Key&, Val&>() const {
            return std::forward_as_tuple(first, second);
        }

        entry* operator->() {
            return this;
        }
    };

    struct const_entry {
        const Key& first;
        const Val& second;

        operator std::tuple<const Key&, const Val&>() const {
            return std::forward_as_tuple(first, second);
        }

        const const_entry* operator->() const {
            return this;
        }
    };

    template <typename EntryType, typename MapPtr>
    class iterator_base {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = EntryType;
        using difference_type = std::ptrdiff_t;
        using pointer = EntryType*;
        using reference = EntryType;

        iterator_base(size_type _idx, MapPtr _map) 
            : idx(_idx), map(_map) {}

        EntryType operator*() const {
            return { map->table[idx].first, map->table[idx].second };
        }

        EntryType operator->() const {
            return { map->table[idx].first, map->table[idx].second };
        }

        iterator_base& operator++() {
            ++idx;
            while (idx < map->table_size && !map->table[idx].occupied)
                ++idx;
            return *this;
        }

        bool operator==(const iterator_base& other) const {
            return idx == other.idx;
        }

        bool operator!=(const iterator_base& other) const {
            return idx != other.idx;
        }

    private:
        size_type idx;
        MapPtr map;
    };

    using iterator = iterator_base<entry, fast_hash_map*>;
    using const_iterator = iterator_base<const_entry, const fast_hash_map*>;

    fast_hash_map(size_type n = 0) : num_elements(0), table_size(0), mask(0) {
        size_type k = 8;
        while (k < (n << 1)) 
            k <<= 1;
        build(k);
    }

    fast_hash_map(const fast_hash_map& other) {
        num_elements = other.num_elements;
        table_size = other.table_size;
        mask = other.mask;

        if (table_size > 0) {
            table.reset(new node[table_size]);
            for (size_type i = 0; i < table_size; ++i) 
                table[i] = other.table[i];
        }
    }

    fast_hash_map(fast_hash_map&& other) noexcept = default;

    fast_hash_map& operator=(const fast_hash_map& other) {
        if (this != &other) {
            num_elements = other.num_elements;
            table_size = other.table_size;
            mask = other.mask;

            if (table_size > 0) {
                table.reset(new node[table_size]);
                for (size_type i = 0; i < table_size; ++i)
                    table[i] = other.table[i];
            } else {
                table.reset();
            }
        }
        return *this;
    }

    fast_hash_map& operator=(fast_hash_map&& other) noexcept = default;

    iterator begin() {
        size_type i = 0;
        while (i < table_size && !table[i].occupied)
            ++i;
        return iterator(i, this);
    }

    iterator end() {
        return iterator(table_size, this);
    }

    const_iterator begin() const {
        size_type i = 0;
        while (i < table_size && !table[i].occupied)
            ++i;
        return const_iterator(i, this);
    }

    const_iterator end() const {
        return const_iterator(table_size, this);
    }

    size_type size() const {
        return num_elements;
    }

    size_type bucket_count() const {
        return table_size;
    }

    bool empty() const {
        return num_elements == 0;
    }

    void clear() {
        if (num_elements > 0) {
            for (size_type i = 0; i < table_size; ++i) 
                table[i].occupied = false;
            num_elements = 0;
        }
    }

    Val& operator[](const Key& key) {
        if ((num_elements << 1) >= table_size)
            rehash(table_size << 1);

        size_type i = find_index(key);
        if (!table[i].occupied) {
            table[i].first = key;
            table[i].second = Val{};
            table[i].occupied = true;
            ++num_elements;
        }

        return table[i].second;
    }

    iterator find(const Key& key) {
        size_type i = find_index(key);
        return table[i].occupied ? iterator(i, this) : end();
    }

    const_iterator find(const Key& key) const {
        size_type i = find_index(key);
        return table[i].occupied ? const_iterator(i, this) : end();
    }

    bool count(const Key& key) const {
        return table[find_index(key)].occupied;
    }

private:
    size_type num_elements;
    size_type table_size;
    size_type mask;
    std::unique_ptr<node[]> table;
    internal::custom_hash hasher;

    void build(size_type k) {
        table_size = k;
        mask = k - 1;
        table.reset(new node[k]);
    }

    inline size_type find_index(const Key& key) const {
        size_type i = hasher(key) & mask;
        while (table[i].occupied && !(table[i].first == key)) {
            i = (i + 1) & mask;
        }
        return i;
    }

    void rehash(size_type new_size) {
        size_type old_size = table_size;
        auto old_table = std::move(table);
        build(new_size);

        num_elements = 0;
        for (size_type i = 0; i < old_size; ++i) {
            if (old_table[i].occupied) {
                size_type j = hasher(old_table[i].first) & mask;
                while (table[j].occupied) j = (j + 1) & mask;
                
                table[j].first = std::move(old_table[i].first);
                table[j].second = std::move(old_table[i].second);
                table[j].occupied = true;
                ++num_elements;
            }
        }
    }
};

} // namespace ds
} // namespace cp
#endif