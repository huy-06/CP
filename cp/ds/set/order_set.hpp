#include <iostream>
#include <ext/pb_ds/assoc_container.hpp>

#ifndef CP_DS_ORDER_SET
#define CP_DS_ORDER_SET
namespace cp {
namespace ds {

template <typename K, typename V>
using map = __gnu_pbds::tree<K, 
                             V, 
                             std::less<K>,
                             __gnu_pbds::rb_tree_tag,
                             __gnu_pbds::tree_order_statistics_node_update>;

template <typename Tp>
using set = map<Tp, __gnu_pbds::null_type>;

template <typename Tp>
class multiset {
public:
    using value_type = Tp;

private:
    using underlying_set_type = set<std::pair<value_type, int>>;
    underlying_set_type data;
    int unique_counter = 0;

public:
    class iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Tp;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;
        using underlying_iterator = typename underlying_set_type::const_iterator;

    private:
        underlying_iterator it;

    public:
        iterator(underlying_iterator it) : it(it) {}

        reference operator*() const { 
            return it->first; 
        }

        pointer operator->() const {
            return &it->first;
        }

        iterator& operator++() {
            ++it;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator& operator--() {
            --it;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return it == other.it;
        }

        bool operator!=(const iterator& other) const {
            return it != other.it;
        }
    };

    using const_iterator = iterator;

    iterator begin() const {
        return iterator(data.begin());
    }

    iterator end() const {
        return iterator(data.end());
    }

    void insert(const value_type& value) {
        data.insert({value, unique_counter++});
    }

    void erase(const value_type& value) {
        auto it = data.lower_bound({value, 0});
        if (it != data.end() && it->first == value) {
            data.erase(it);
        }
    }
    
    iterator erase(iterator pos) {
        return iterator(data.erase(pos.it));
    }

    iterator find(const value_type& value) const {
        auto it = lower_bound(value);
        if (it != end() && *it == value) {
            return it;
        }
        return end();
    }

    iterator lower_bound(const value_type& value) const {
        return iterator(data.lower_bound({value, 0}));
    }

    iterator upper_bound(const value_type& value) const {
        return iterator(data.lower_bound({value, unique_counter}));
    }

    size_t count(const value_type& value) const {
        return data.order_of_key({value, unique_counter}) - data.order_of_key({value, 0});
    }

    size_t order_of_key(const value_type& key) const {
        return data.order_of_key({key, 0});
    }

    value_type find_by_order(const size_t& k) const {
        if (k >= data.size()) {
            throw std::out_of_range("Index out of bounds in multiset::find_by_order");
        }
        return data.find_by_order(k)->first;
    }

    size_t size() const {
        return data.size();
    }
    
    bool empty() const {
        return data.empty();
    }

    void clear() {
        data.clear();
        unique_counter = 0;
    }
};


} // namespace ds
} // namespace cp
#endif