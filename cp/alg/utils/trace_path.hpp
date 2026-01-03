#include <vector>
#include <algorithm>

#ifndef CP_ALG_SP_GRAPH_TRACE
#define CP_ALG_SP_GRAPH_TRACE
namespace cp {
namespace alg {

std::vector<int> trace_path(std::vector<int>& par, int src, int dst) {
    std::vector<int> path;
    path.reserve(par.size());
    
    int v = dst;
    while (v != -1) {
        path.push_back(v);
        if (v == src) {
            break;
        }
        v = par[v];
    }

    path.shrink_to_fit();
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
    path.reserve(par.size() * par[0].size());

    int cur = dst;
    while (cur != src) {
        path.push_back(cur);
        cur = par[src][cur];
        if (cur == -1) {
            return {};
        }
    }
    path.push_back(src);

    path.shrink_to_fit();
    std::reverse(path.begin(), path.end());

    return path;
}

std::vector<std::pair<int, int>> trace_path(
    const std::vector<std::vector<std::pair<int, int>>>& par,
    int xs, int ys,
    int xe, int ye) {

    if (par[xe][ye].first == -1) {
        return {};
    }

    std::vector<std::pair<int, int>> path;
    path.reserve(par.size() * par[0].size());

    int x = xe;
    int y = ye;

    while (x != -1 && y != -1) {
        path.push_back({x, y});

        if (x == xs && y == ys) {
            break;
        }

        int nx, ny;
        std::tie(nx, ny) = par[x][y];
        x = nx;
        y = ny;
    }

    path.shrink_to_fit();
    std::reverse(path.begin(), path.end());

    return path;
}

} // namespace alg
} // namespace cp
#endif