#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

const int MAXN = 100005; // Kích thước tối đa của mảng
const int MAX_NODES = MAXN * 20; // Số lượng nút tối đa cho PST

// Cấu trúc một nút trên cây
struct Node {
    int count;      // Số lượng phần tử trong đoạn này
    int left_child; // Con trỏ (chỉ số) đến nút con trái
    int right_child;// Con trỏ (chỉ số) đến nút con phải
};

Node tree[MAX_NODES];
int root[MAXN];      // Lưu gốc của các phiên bản cây
int a[MAXN];         // Mảng ban đầu
vector<int> distinct_vals; // Lưu các giá trị đã được rời rạc hóa
int node_count = 0; // Đếm số nút đã tạo

// Hàm ánh xạ giá trị thực sang giá trị đã rời rạc hóa
int get_id(int val) {
    return lower_bound(distinct_vals.begin(), distinct_vals.end(), val) - distinct_vals.begin();
}

// Hàm cập nhật: tạo một phiên bản cây mới từ phiên bản cũ
// prev_node_idx: chỉ số của nút ở phiên bản trước
// l, r: đoạn giá trị mà nút hiện tại quản lý
// pos: vị trí (đã rời rạc hóa) của giá trị cần cập nhật
int update(int prev_node_idx, int l, int r, int pos) {
    int current_node_idx = ++node_count; // Tạo nút mới
    tree[current_node_idx] = tree[prev_node_idx]; // Sao chép thông tin từ nút cũ
    tree[current_node_idx].count++; // Tăng số lượng phần tử

    if (l == r) {
        return current_node_idx;
    }

    int mid = l + (r - l) / 2;
    if (pos <= mid) {
        // Nếu giá trị cần cập nhật nằm ở nhánh trái
        tree[current_node_idx].left_child = update(tree[prev_node_idx].left_child, l, mid, pos);
    } else {
        // Nếu giá trị cần cập nhật nằm ở nhánh phải
        tree[current_node_idx].right_child = update(tree[prev_node_idx].right_child, mid + 1, r, pos);
    }
    return current_node_idx;
}

// Hàm truy vấn: tìm phần tử nhỏ thứ k
// u_idx: nút ở phiên bản r
// v_idx: nút ở phiên bản l-1
// l, r: đoạn giá trị mà nút hiện tại quản lý
// k: thứ tự của phần tử cần tìm
int query(int u_idx, int v_idx, int l, int r, int k) {
    if (l == r) {
        return l; // Đã tìm thấy nút lá, trả về vị trí
    }

    // Số lượng phần tử ở nhánh trái trong đoạn [l, r]
    int left_count = tree[tree[u_idx].left_child].count - tree[tree[v_idx].left_child].count;

    int mid = l + (r - l) / 2;
    if (k <= left_count) {
        // Phần tử thứ k nằm ở nhánh trái
        return query(tree[u_idx].left_child, tree[v_idx].left_child, l, mid, k);
    } else {
        // Phần tử thứ k nằm ở nhánh phải
        return query(tree[u_idx].right_child, tree[v_idx].right_child, mid + 1, r, k - left_count);
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, q;
    cin >> n >> q;

    for (int i = 1; i <= n; ++i) {
        cin >> a[i];
        distinct_vals.push_back(a[i]);
    }

    // Rời rạc hóa giá trị
    sort(distinct_vals.begin(), distinct_vals.end());
    distinct_vals.erase(unique(distinct_vals.begin(), distinct_vals.end()), distinct_vals.end());

    int m = distinct_vals.size();

    // Xây dựng các phiên bản của cây
    // root[0] là cây rỗng
    root[0] = 0; // Nút 0 được coi là nút null
    tree[0] = {0, 0, 0};

    for (int i = 1; i <= n; ++i) {
        int pos = get_id(a[i]);
        root[i] = update(root[i - 1], 0, m - 1, pos);
    }

    // Xử lý các truy vấn
    for (int i = 0; i < q; ++i) {
        int l, r, k;
        cin >> l >> r >> k;
        int result_pos = query(root[r], root[l - 1], 0, m - 1, k);
        cout << distinct_vals[result_pos] << "\n";
    }

    return 0;
}