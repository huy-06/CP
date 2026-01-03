#include "../../ds/string/suffix_automaton.hpp"

#ifndef CP_ALG_STRING_LCS
#define CP_ALG_STRING_LCS
namespace cp {
namespace alg {

/// @brief Tìm khoảng [l, r) của xâu con chung dài nhất dựa trên SAM đã dựng sẵn.
template <typename Tp, typename Array>
std::pair<int, int> lcs_with_sam(const ds::suffix_automaton<Tp>& sam, const Array& t) {
    int v = 0;
    int cur_l = 0;
    int max_l = 0;
    int end_pos = -1;

    for (int i = 0; i < int(t.size()); ++i) {
        Tp c = static_cast<Tp>(t[i]);

        while (v != 0 && sam.states[v].get_next(c) == -1) {
            v = sam.states[v].link;
            cur_l = sam.states[v].len;
        }

        int next_states = sam.states[v].get_next(c);
        if (next_states != -1) {
            v = next_states;
            ++cur_l;
        } else {
            v = 0;
            cur_l = 0;
        }

        if (cur_l > max_l) {
            max_l = cur_l;
            end_pos = i;
        }
    }

    if (max_l == 0) {
        return {0, 0};
    }
    return {end_pos - max_l + 1, end_pos + 1};
}

/// @brief Tìm xâu con chung dài nhất của hai xâu s và t.
template <typename Array>
Array lcs(const Array& s, const Array& t) {
    if (s.empty() || t.empty()) return Array();

    using type = typename Array::value_type;

    ds::suffix_automaton<type> sam(s);

    int l, r;
    std::tie(l, r) = lcs_with_sam(sam, t);

    if (l == r) {
        return Array();
    }
    return Array(t.begin() + l, t.begin() + r);
}

} // namespace alg
} // namespace cp
#endif