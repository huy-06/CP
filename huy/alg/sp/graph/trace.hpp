#include <vector>
#include <algorithm>

#ifndef HUY_ALG_SP_GRAPH_TRACE
#define HUY_ALG_SP_GRAPH_TRACE
namespace huy {
namespace alg {

std::vector<int> trace_path(std::vector<int>& par, int src, int dst) {
    std::vector<int> path;
    int v = dst;
    while (v != -1) {
        path.push_back(v);
        if (v == src) {
            break;
        }
        v = par[v];
    }
    std::reverse(path.begin(), path.end());
    if (path.front() != src) {
        return {};
    }
    return path;
}

std::vector<int> trace_path(std::vector<std::vector<int>>& par, int src, int dst) {
    if (par[src][dst] == -1) {
        return {};
    }
    std::vector<int> path;
    int cur = dst;
    while (cur != src) {
        path.push_back(cur);
        cur = par[src][cur];
        if (cur == -1) {
            return {};
        }
    }
    path.push_back(src);
    std::reverse(path.begin(), path.end());
    return path;
}

} // namespace alg
} // namespace huy
#endif