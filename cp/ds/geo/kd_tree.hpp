#include "../../alg/geo/distance.hpp"

#ifndef CP_DS_KD_TREE
#define CP_DS_KD_TREE
namespace cp {
namespace ds {

template <typename Point, auto dist_func>
class kd_tree {
public:
    using point_type = Point;
    using value_type = typename Point::value_type;

    kd_tree() = default;

    explicit kd_tree(const std::vector<point_type>& points) {
        build(points);
    }

    void clear() {
        tree.clear();
    }

    void build(const std::vector<point_type>& points) {
        clear();
        if (points.empty()) return;

        tree.reserve(points.size());
        std::vector<point_type> pts = points;
        build_rec(pts, 0, static_cast<int>(pts.size()) - 1);
    }

    std::pair<value_type, point_type> nearest(const point_type& target) const {
        std::pair<value_type, point_type> best;
        best.first = std::numeric_limits<value_type>::has_infinity 
                    ? std::numeric_limits<value_type>::infinity() 
                    : std::numeric_limits<value_type>::max();
        best.second = point_type();

        if (!empty()) {
            search_rec(0, target, best);
        }
        return best;
    }

    int size() const { 
        return static_cast<int>(tree.size());
    }

    bool empty() const {
        return tree.empty();
    }

private:
    struct node {
        point_type p;
        int axis;
        int l = -1;
        int r = -1;
    };

    std::vector<node> tree;

    value_type get_coord(const point_type& p, int axis) const {
        return axis == 0 ? p.x : p.y;
    }

    point_type project_to_axis(const point_type& target, const point_type& node_pt, int axis) const {
        if (axis == 0) {
            return point_type(node_pt.x, target.y);
        }
        return point_type(target.x, node_pt.y);
    }

    int build_rec(std::vector<point_type>& pts, int l, int r) {
        if (l > r) return -1;

        value_type min_x = pts[l].x, max_x = pts[l].x;
        value_type min_y = pts[l].y, max_y = pts[l].y;

        for (int i = l + 1; i <= r; ++i) {
            min_x = std::min(min_x, pts[i].x);
            max_x = std::max(max_x, pts[i].x);
            min_y = std::min(min_y, pts[i].y);
            max_y = std::max(max_y, pts[i].y);
        }

        int axis = (max_y - min_y > max_x - min_x) ? 1 : 0;
        int mid = l + (r - l) / 2;

        std::nth_element(pts.begin() + l, pts.begin() + mid, pts.begin() + r + 1,
            [this, axis](const point_type& a, const point_type& b) {
                return get_coord(a, axis) < get_coord(b, axis);
            });

        int idx = static_cast<int>(tree.size());
        tree.push_back({pts[mid], axis, -1, -1});

        int l_child = build_rec(pts, l, mid - 1);
        int r_child = build_rec(pts, mid + 1, r);

        tree[idx].l = l_child;
        tree[idx].r = r_child;

        return idx;
    }

    void search_rec(int u, const point_type& target, std::pair<value_type, point_type>& best) const {
        if (u == -1) return;

        const auto& cur = tree[u];
        value_type d = dist_func(target, cur.p);

        if (d < best.first) {
            best.first = d;
            best.second = cur.p;
        }

        int axis = cur.axis;
        value_type diff = get_coord(target, axis) - get_coord(cur.p, axis);

        int fi = diff <= 0 ? cur.l : cur.r;
        int se = diff <= 0 ? cur.r : cur.l;

        search_rec(fi, target, best);

        value_type axis_dist = dist_func(target, project_to_axis(target, cur.p, axis));
        if (axis_dist < best.first) {
            search_rec(se, target, best);
        }
    }
};

} // namespace ds
} // namespace cp
#endif