#include <cstdint>
#include <immintrin.h>
#include <iterator>
#include <utility>
#include <memory>
#include <cstdlib>
#include <functional>

#ifndef CP_DS_HASH_COMPACT_FLAT_MAP
#define CP_DS_HASH_COMPACT_FLAT_MAP
namespace cp {
namespace ds {

template <typename K, typename V>
class compact_flat_map {
public:
    using value_type = std::pair<K, V>;
    using iterator = typename std::vector<value_type>::iterator;
    using const_iterator = typename std::vector<value_type>::const_iterator;

    iterator begin() {
        return data.begin();
    }

    iterator end() {
        return data.end();
    }

    const_iterator begin() const {
        return data.begin();
    }

    const_iterator end() const {
        return data.end();
    }

    const_iterator cbegin() const {
        return data.cbegin();
    }

    const_iterator cend() const {
        return data.cend();
    }

    bool empty() const {
        return data.empty();
    }

    size_t size() const {
        return data.size();
    }

    void reserve(size_t new_cap) {
        data.reserve(new_cap);
    }
    
    void shrink_to_fit() {
        data.shrink_to_fit();
    }

    void clear() {
        data.clear();
    }    

     V& operator[](const K& key) {
        auto it = get_lower_bound(key);
        if (it != data.end() && it->first == key) {
            return it->second;
        }
        it = data.insert(it, {key, V{}});
        return it->second;
    }

    V& at(const K& key) {
        auto it = get_lower_bound(key);
        if (it != data.end() && it->first == key) {
            return it->second;
        }
        throw std::out_of_range("compact_flat_map::at: key not found");
    }

    std::pair<iterator, bool> insert(const value_type& value) {
        auto it = get_lower_bound(value.first);
        if (it != data.end() && it->first == value.first) {
            return {it, false};
        }
        it = data.insert(it, value);
        return {it, true};
    }

    size_t erase(const K& key) {
        auto it = get_lower_bound(key);
        if (it != data.end() && it->first == key) {
            data.erase(it);
            return 1;
        }
        return 0;
    }

    iterator find(const K& key) {
        auto it = get_lower_bound(key);
        if (it != data.end() && it->first == key) {
            return it;
        }
        return data.end();
    }

    const_iterator find(const K& key) const {
        auto it = get_lower_bound(key);
        if (it != data.end() && it->first == key) {
            return it;
        }
        return data.end();
    }

    bool contains(const K& key) const {
        auto it = get_lower_bound(key);
        return it != data.end() && it->first == key;
    }

    iterator lower_bound(const K& key) {
        return get_lower_bound(key);
    }

    iterator upper_bound(const K& key) {
        return std::upper_bound(data.begin(), data.end(), key,
            [](const K& k, const value_type& pair) {
                return k < pair.first;
            }
        );
    }

private:
    std::vector<value_type> data;

    auto get_lower_bound(const K& key) {
        return std::lower_bound(data.begin(), data.end(), key,
            [](const value_type& pair, const K& k) {
                return pair.first < k;
            }
        );
    }

    auto get_lower_bound(const K& key) const {
        return std::lower_bound(data.begin(), data.end(), key,
            [](const value_type& pair, const K& k) {
                return pair.first < k;
            }
        );
    }
};

} // namespace ds
} // namespace cp
#endif