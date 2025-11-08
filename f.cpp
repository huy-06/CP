/// @author Huy
/// @since  19:57:02 03/11/2025
#include "huy/init/lib.hpp"
#include <ext/pb_ds/assoc_container.hpp>

using set = __gnu_pbds::tree<std::pair<int, int>, __gnu_pbds::null_type, std::less<std::pair<int, int>>, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;

signed main() {
	//! std::ios_base::sync_with_stdio(false);
	//! std::cin.tie(nullptr);
	//! std::cout.tie(nullptr);

	int n, q;
	std::cin >> n >> q;

	int l1, l2;
	std::cin >> l1 >> l2;

	std::vector<int> a(n);
	for (int i = 0; i < n; ++i) {
		std::cin >> a[i];
	}

	std::vector<std::array<int, 3>> queries(q);
	std::vector<std::vector<std::pair<int, int>>> i_kth_l1(n);
	std::vector<std::vector<std::pair<int, int>>> i_kth_l2(n);	

	for (int i = 0; i < q; ++i) {
		int l, j, k;
		std::cin >> l >> j >> k;
		--j;
		queries[i] = {l, j, k};
		if (l == 1) {
			i_kth_l1[j].push_back({k, i});
		} else {
			i_kth_l2[j].push_back({k, i});
		}
	}

	std::vector<std::array<int, 3>> ans(q);
	{
		set se;
		for (int i = n - 1; i >= n - l1 + 1; --i) {
			se.insert({a[i], i});
		}

		for (int i = n - l1; i >= 0; --i) {
			se.insert({a[i], i});
			if (!i_kth_l1[i].empty()) {
				auto it_min = *se.begin();
				int min = it_min.first;
				auto it_max = *se.rbegin();
				int max = it_max.first;
				for (auto [k, i_query] : i_kth_l1[i]) {
					auto it_kth = *se.find_by_order(k - 1);
					int kth = it_kth.first;
					ans[i_query] = {min, kth, max};
				}
			}
			int i_erase = i + l1 - 1;
			se.erase({a[i_erase], i_erase});
		}
	}
	{
		set se;
		for (int i = n - 1; i >= n - l2 + 1; --i) {
			se.insert({a[i], i});
		}

		for (int i = n - l2; i >= 0; --i) {
			se.insert({a[i], i});
			if (!i_kth_l2[i].empty()) {
				auto it_min = *se.begin();
				int min = it_min.first;
				auto it_max = *se.rbegin();
				int max = it_max.first;
				for (auto [k, i_query] : i_kth_l2[i]) {
					auto it_kth = *se.find_by_order(k - 1);
					int kth = it_kth.first;
					ans[i_query] = {min, kth, max};
				}
			}
			int i_erase = i + l2 - 1;
			se.erase({a[i_erase], i_erase});
		}
	}
	
	for (auto [min, kth, max] : ans) {
		std::cout << min << ' ' << kth << ' ' << max << '\n';
	}

	return 0;
}