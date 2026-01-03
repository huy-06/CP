#include "../hash/fast_hash_map.hpp"

#ifndef CP_DS_STRING_SUFFIX_AUTOMATON
#define CP_DS_STRING_SUFFIX_AUTOMATON
namespace cp {
namespace ds {

template <typename Tp>
class suffix_automaton {
public:
    using value_type = Tp;

    class state {
    public:
        int len, link;
        cp::ds::fast_hash_map<value_type, int> next;

        long long cnt = 0;
        int first_pos = -1;
        bool is_clone = false;

        int get_next(const value_type& c) const {
            auto it = next.find(c);
            if (it == next.end()) {
                return -1;
            }
            return it->second;
        }

        void set_next(const value_type& c, const int& val) {
            next[c] = val;
        }
    };


    std::vector<state> states;
    int last;

    suffix_automaton() {
        states.clear();
        states.push_back({0, -1});
        last = 0;
    }

    suffix_automaton(const std::vector<value_type>& v) : suffix_automaton() {
        states.reserve(v.size() << 1);
        for (const auto& x : v) {
            extend(x);
        }
    }

    suffix_automaton(const std::string& s) : suffix_automaton() {
        states.reserve(s.size() << 1);
        for (const auto& x : s) {
            extend(static_cast<value_type>(x));
        }
    }

    void extend(const value_type& c) {
        int cur = states.size();
        states.emplace_back();

        states[cur].len = states[last].len + 1;
        states[cur].cnt = 1;
        states[cur].first_pos = states[cur].len - 1;
        states[cur].is_clone = false;

        int p = last;
        while (p != -1 && states[p].get_next(c) == -1) {
            states[p].set_next(c, cur);
            p = states[p].link;
        }

        if (p == -1) {
            states[cur].link = 0;
        } else {
            int q = states[p].get_next(c);
            if (states[p].len + 1 == states[q].len) {
                states[cur].link = q;
            } else {
                int clone = states.size();
                states.emplace_back();
                
                states[clone].len = states[p].len + 1;
                states[clone].next = states[q].next;
                states[clone].link = states[q].link;
                states[clone].cnt = 0;
                states[clone].is_clone = true;
                states[clone].first_pos = states[q].first_pos;

                while (p != -1 && states[p].get_next(c) == q) {
                    states[p].set_next(c, clone);
                    p = states[p].link;
                }
                states[q].link = states[cur].link = clone;
            }
        }

        last = cur;
    }

    void calc_occurrences() {
        std::vector<int> nodes(states.size());
        for (int i = 0; i < int(states.size()); ++i) {
            nodes[i] = i;
        }
        std::sort(nodes.begin(), nodes.end(), [&](int a, int b) {
            return states[a].len > states[b].len;
        });

        for (int u : nodes) {
            if (states[u].link != -1) {
                states[states[u].link].cnt += states[u].cnt;
            }
        }
    }
};

} // namespace ds
} // namespace cp
#endif