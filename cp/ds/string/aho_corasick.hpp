#include <queue>
#include "../hash/fast_hash_map.hpp"

#ifndef CP_DS_STRING_AHO_CORASICK
#define CP_DS_STRING_AHO_CORASICK
namespace cp {
namespace ds {

template <typename Tp>
class aho_corasick {
public:
    using value_type = Tp;

    class state {
    public:
        ds::fast_hash_map<value_type, int> next;
        int link = 0;
        int min_len = INT_MAX;

        // int get_next(const value_type& c) const {
        //     auto it = next.find(c);
        //     if (it == next.end()) {
        //         return -1;
        //     }
        //     return it->second;
        // }
    };

    std::vector<state> states;

    aho_corasick() {
        states.emplace_back();
    }

    void clear() {
        states.clear();
        states.emplace_back();
    }

    template <typename Array>
    void insert(const Array& a) {
        int u = 0;
        for (int i = 0; i < int(a.size()); ++i) {
            value_type v = static_cast<value_type>(a[i]);
            if (states[u].next.find(v) == states[u].next.end()) {
                states[u].next[v] = states.size();
                states.emplace_back();
            }
            u = states[u].next[v];
        }
        states[u].min_len = std::min(states[u].min_len, int(a.size()));
    }

    void build() {
        std::queue<int> q;
        
        for (const auto& edge : states[0].next) {
            q.push(edge.second);
        }

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (const auto& edge : states[u].next) {
                value_type ch = edge.first;
                int v = edge.second;

                int f = states[u].link;
                while (f > 0 && states[f].next.find(ch) == states[f].next.end()) {
                    f = states[f].link;
                }

                if (states[f].next.find(ch) != states[f].next.end()) {
                    states[v].link = states[f].next[ch];
                } else {
                    states[v].link = 0;
                }

                states[v].min_len = std::min(states[v].min_len, states[states[v].link].min_len);
                q.push(v);
            }
        }
    }

    int transition(int u, value_type c) const {
        while (u > 0 && states[u].next.find(c) == states[u].next.end()) {
            u = states[u].link;
        }
        auto it = states[u].next.find(c);
        if (it != states[u].next.end()) {
            return it->second;
        }
        return 0;
    }
};

} // namespace ds
} // namespace cp
#endif