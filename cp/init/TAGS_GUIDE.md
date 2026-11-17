# CẨM NANG SỬ DỤNG CÁC LỆNH TAG `[...]` TRONG COMMENT (CP RUNNER)

> Tài liệu hướng dẫn chi tiết cách sử dụng các thẻ lệnh (tags) đặt trong comment của mã nguồn C++ khi chạy với **Runner**. Các thẻ tag này giúp bạn tự động hóa việc test bài, so sánh đáp án, stress test, kiểm tra bài tương tác và gộp code (bundler).

---

## 📌 BẢNG TRA CỨU NHANH TẤT CẢ CÁC TAG

| Tag | Ý nghĩa / Chức năng | Ví dụ cú pháp |
| :--- | :--- | :--- |
| **`[IN]`** / **`[IN: <file>]`** | Khối dữ liệu đầu vào (inline hoặc đọc từ file `.in`) | `// [IN]` hoặc `// [IN: 1.in]` |
| **`[OUT]`** / **`[OUT: <file>]`** | Khối dữ liệu đầu ra kỳ vọng (inline hoặc đọc từ file `.out`) | `// [OUT]` hoặc `// [OUT: 1.out]` |
| **`[TESTS_DIR: <dir>]`** | Tự động quét và chạy toàn bộ test trong thư mục (flat hoặc subfolder) | `// [TESTS_DIR: tests]` |
| **`[-IN]` / `[-OUT]`** | **Tắt (skip)** test case đó, runner sẽ bỏ qua không chạy | `// [-IN]` hoặc `// [-IN: 1.in]` |
| **`[GEN: <file> [n]]`** | Chỉ định file sinh test ngẫu nhiên (chạy `n` test) | `// [GEN: gen.cpp 100]` |
| **`[BRUTE: <file>]`** | Chỉ định file giải trâu (chuẩn) để so sánh đáp án | `// [BRUTE: brute.cpp]` |
| **`[CHECKER: <file>]`** | Chỉ định file checker (cho bài nhiều nghiệm, đồ thị, float) | `// [CHECKER: checker.cpp]` |
| **`[INTERACTIVE: <file> [n]]`** | Chạy chế độ bài toán tương tác 2 chiều với Interactor | `// [INTERACTIVE: interactor.cpp]` |
| **`[JUDGE: <file> [n]]`** | Chạy chế độ Custom Judge tự quản lý sinh test & chấm | `// [JUDGE: my_judge.cpp 50]` |
| **`[MANUAL]`** | Chạy thủ công: nhập dữ liệu trực tiếp từ bàn phím | `// [MANUAL]` hoặc `// [MODE: MANUAL]` |
| **`[RUN-ONLY]`** | Chạy qua các test `[IN]` nhưng không so sánh với `[OUT]` | `// [RUN-ONLY]` |
| **`[EVALUATE]` / `[EVAL]`** | Chạy đánh giá thời gian / tài nguyên | `// [EVALUATE]` |
| **`[TL: <giây>]`** | Thiết lập Time Limit (mặc định 5.0 giây) | `// [TL: 2.0]` hoặc `// [TL: 1.5]` |
| **`[EPS: <sai số>]`** | Thiết lập sai số so sánh số thực float | `// [EPS: 1e-6]` hoặc `// [EPS: 1e-9]` |
| **`[SAVE]` / `[SAVE: <dir>]`** | Tự động lưu test bị lỗi (WA, TLE, RTE) ra thư mục | `// [SAVE]` hoặc `// [SAVE: "wa_tests"]` |
| **`[NO-SAVE]`** | Tắt tính năng lưu test bị lỗi | `// [NO-SAVE]` |
| **`[BUNDLE-ONLY]`** | Chỉ gộp file `submit/submit.cpp`, không chạy test | `// [BUNDLE-ONLY]` |
| **`[NO-BUNDLE]`** | Tắt tính năng tự động tạo bundle | `// [NO-BUNDLE]` |
| **`[NO-DEBUG]`** | Không hiển thị output từ kênh `std::cerr` / `debug(...)` | `// [NO-DEBUG]` |
| **`[NO-LOG]`** | Không hiển thị log chi tiết khi test | `// [NO-LOG]` |
| **`[FILE-IN: "<tên>"]`** | Chuyển hướng stdin đọc từ file (freopen) | `// [FILE-IN: "input.txt"]` |
| **`[FILE-OUT: "<tên>"]`** | Chuyển hướng stdout ghi vào file | `// [FILE-OUT: "output.txt"]` |
| **`[-TAG]`** | Bất kỳ tag nào có dấu `-` đằng trước đều sẽ bị **vô hiệu hóa** | `// [-GEN: gen.cpp]`, `// [-TL: 1.0]` |

---

## 🎯 CHI TIẾT CÁCH SỬ DỤNG VÀ CODE MẪU

### 1. Test Case Cơ Bản (`[IN]` và `[OUT]`)

* Bạn có thể viết nhiều cặp `[IN]` và `[OUT]` trong cùng một file.
* Muốn tạm thời không chạy test case nào, chỉ cần đổi `[IN]` thành `[-IN]`.
* Khi test bị sai (WA/TLE), Runner sẽ tự động chèn test lỗi vào comment cuối file để bạn debug.

```cpp
#include <iostream>

// [IN]
// 3
// 1 2 3
// [OUT]
// 6

// [-IN] Test này sẽ bị bỏ qua không chạy
// 2
// 10 20
// [-OUT]
// 30

// [TL: 1.0] Giới hạn thời gian 1 giây

int main() {
    int n;
    if (!(std::cin >> n)) return 0;
    long long sum = 0;
    for (int i = 0; i < n; ++i) {
        long long x;
        std::cin >> x;
        sum += x;
    }
    std::cout << sum << "\n";
    return 0;
}
```

---

### 1.1. Đọc Test Từ File Rời (`[IN: file]` và `[OUT: file]`)

* Hỗ trợ đọc input / output từ file độc lập và kết hợp tự do (cross-matching):
  * `[IN: file.in]` kết hợp với `[OUT]` inline (viết trực tiếp trong comment).
  * `[IN]` inline kết hợp với `[OUT: file.out]`.
  * `[IN: file.in]` kết hợp với `[OUT: file.out]`.
* Tự động kiểm tra file tồn tại; nếu file không tồn tại sẽ báo lỗi đỏ trực quan ngay từ đầu.
* Khi chấm, tên file test được hiển thị rõ ràng trên từng dòng kết quả và phần diff.

```cpp
/*
[IN: custom.in]
[OUT]
6

[IN]
4
1 2 3 4
[OUT: custom.out]

[IN: 1.in]
[OUT: 1.out]
*/
```

---

### 1.2. Tự Động Quét Toàn Bộ Thư Mục Test (`[TESTS_DIR: folder]`)

* Tự động duyệt và ghép cặp toàn bộ test trong một thư mục chỉ với 1 dòng: `[TESTS_DIR: tests]`.
* **Hỗ trợ 2 kiểu cấu trúc phổ biến**:
  1. **Subfolder-per-test (Dạng đề thi HSG, Themis, Dmoj)**:
     `tests/test01/data.in` và `tests/test01/data.out` (hoặc `.ans`).
  2. **Flat files (Dạng Codeforces, CSES, HackerRank)**:
     `tests/1.in` và `tests/1.out` (hoặc `.ans`).
* **Sắp xếp tự nhiên (Natural Sorting)**: Test được sắp xếp theo đúng thứ tự số thực tế: `1, 2, ..., 9, 10, 11` (không bị đảo thứ tự chuỗi `1, 10, 2`).
* Có thể tùy biến đuôi file nếu cần: `[TESTS_DIR: tests (*.inp, *.out)]`.

```cpp
/*
[TESTS_DIR: tests]
*/
```

---

### 2. Stress Test Tự Động (`[GEN]` và `[BRUTE]`)

* Tự động sinh test ngẫu nhiên từ file `gen.cpp` và so khớp output của code hiện tại với file `brute.cpp`.
* Thêm số lượng test muốn chạy sau tên file: `[GEN: gen.cpp 200]`.

#### File chính (`sol.cpp`):
```cpp
#include <iostream>
#include <vector>
#include <algorithm>

// [GEN: gen.cpp 100]    Chạy 100 test ngẫu nhiên
// [BRUTE: brute.cpp]    So với code trâu
// [TL: 1.0]

int main() {
    int n;
    if (!(std::cin >> n)) return 0;
    std::vector<int> a(n);
    for (int i = 0; i < n; ++i) std::cin >> a[i];
    std::sort(a.begin(), a.end());
    std::cout << a.back() << "\n";
    return 0;
}
```

#### File sinh test (`gen.cpp`):
```cpp
#include <iostream>
#include <random>
#include <chrono>

int main() {
    std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    int n = rng() % 10 + 1;
    std::cout << n << "\n";
    for (int i = 0; i < n; ++i) {
        std::cout << (rng() % 100 + 1) << (i + 1 == n ? "" : " ");
    }
    std::cout << "\n";
    return 0;
}
```

#### File giải trâu (`brute.cpp`):
```cpp
#include <iostream>

int main() {
    int n;
    if (!(std::cin >> n)) return 0;
    int mx = -1e9;
    for (int i = 0; i < n; ++i) {
        int x;
        std::cin >> x;
        if (x > mx) mx = x;
    }
    std::cout << mx << "\n";
    return 0;
}
```

---

### 3. Custom Checker / So Sánh Số Thực (`[CHECKER]`)

* Dùng cho bài toán **nhiều đáp án đúng** (Special Judge), bài đồ thị hoặc so sánh số thực với sai số `[EPS: ...]`.
* Trong file checker, ta dùng namespace `cp::init::judge` (alias ngắn gọn: `namespace judge = cp::init::judge;`).

#### File bài làm (`sol.cpp`):
```cpp
#include <iostream>

// [IN]
// 5
// [OUT]
// 2.2360679775

// [CHECKER: checker.cpp]
// [EPS: 1e-6]

int main() {
    double n;
    std::cin >> n;
    // In căn bậc 2
    std::cout << __builtin_sqrt(n) << "\n";
    return 0;
}
```

#### File kiểm tra (`checker.cpp`):
> Runner sẽ truyền cho checker:
> * `argv[1]`: Đường dẫn file test input.
> * `argv[2]`: Đường dẫn file output thực tế của solution.
> * `argv[3]`: Đường dẫn file output kỳ vọng (từ tag `[OUT]`).

```cpp
#include <iostream>
#include <fstream>
#include <string>
#include "init/judge.hpp"

namespace judge = cp::init::judge;

int main(int argc, char* argv[]) {
    if (argc < 4) return 1;

    std::ifstream f_in(argv[1]);
    std::ifstream f_out(argv[2]);
    std::ifstream f_ans(argv[3]);

    double user_val, ans_val;
    if (!(f_out >> user_val)) {
        judge::wrong_answer("Khong doc duoc output cua thi sinh!"); // Báo WA (exit 1)
    }
    f_ans >> ans_val;

    // So sánh sai số tuyệt đối hoặc tương đối
    if (judge::equal_tokens(std::to_string(user_val), std::to_string(ans_val), 1e-6)) {
        judge::accept(); // Báo AC (exit 0)
    } else {
        judge::wrong_answer("Sai so vuot qua EPS cho phep!");
    }
}
```

---

### 4. Bài Toán Tương Tác (`[INTERACTIVE]`)

* Dùng cho các bài toán tương tác (Interactive Problems).
* Solution và Interactor sẽ kết nối trực tiếp với nhau qua đường ống 2 chiều (Pipes).
* Khi solution in câu hỏi `std::cout << ... << std::endl;`, Interactor sẽ đọc từ `std::cin` và trả lời lại qua `std::cout`.

#### File bài làm (`sol.cpp`):
```cpp
#include <iostream>
#include <string>

// [INTERACTIVE: interactor.cpp]
// [TL: 2.0]

int main() {
    int low = 1, high = 100;
    while (low <= high) {
        int mid = (low + high) / 2;
        std::cout << mid << std::endl; // Bắt buộc phải flush hoặc dùng std::endl

        std::string response;
        if (!(std::cin >> response)) break;

        if (response == "CORRECT") {
            break;
        } else if (response == "GREATER") {
            low = mid + 1;
        } else if (response == "LESS") {
            high = mid - 1;
        }
    }
    return 0;
}
```

#### File tương tác (`interactor.cpp`):
```cpp
#include <iostream>
#include <string>
#include <cstdlib>

int main() {
    int secret = 42; // Số bí mật cần đoán
    int queries = 0;
    const int max_queries = 10;

    int guess;
    while (std::cin >> guess) {
        ++queries;
        if (queries > max_queries) {
            std::cerr << "Qua so lan hoi cho phep!\n";
            return 1; // Báo WA
        }

        if (guess == secret) {
            std::cout << "CORRECT" << std::endl;
            return 0; // Báo AC
        } else if (guess < secret) {
            std::cout << "GREATER" << std::endl;
        } else {
            std::cout << "LESS" << std::endl;
        }
    }
    return 1;
}
```

---

### 5. Chế Độ Custom Judge Toàn Diện (`[JUDGE]`)

* File judge tự động sinh test và gọi chương trình con solution thông qua hàm `judge::run(input)` hoặc `judge::call(input)`.
* **Đặc tính tự động**: Nếu solution bị **TLE** hoặc **RTE**, hàm `judge::run()` sẽ **tự động exit ngay với mã lỗi tương ứng**, bạn **không cần viết code kiểm tra TLE hay RTE**.

#### File bài làm (`sol.cpp`):
```cpp
#include <iostream>

// [JUDGE: my_judge.cpp 50] Chạy 50 test case
// [TL: 1.0]

int main() {
    int a, b;
    if (std::cin >> a >> b) {
        std::cout << (a + b) << "\n";
    }
    return 0;
}
```

#### File Judge (`my_judge.cpp`):
```cpp
#include <iostream>
#include <random>
#include <chrono>
#include "init/judge.hpp"

namespace judge = cp::init::judge;

int main() {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    // 1. Tự sinh test ngẫu nhiên
    int a = rng() % 1000;
    int b = rng() % 1000;
    std::string input = std::to_string(a) + " " + std::to_string(b) + "\n";

    // 2. Chạy solution (Tự động exit(3) nếu TLE, tự động exit(2) nếu RTE)
    std::string output = judge::run(input);

    // 3. Kiểm tra đáp án
    int expected = a + b;
    int actual = std::stoi(output);

    if (actual != expected) {
        judge::wrong_answer(); // Báo WA (exit 1)
    }

    judge::accept(); // Báo AC (exit 0)
}
```

---

## 💡 MẸO SỬ DỤNG HỮU ÍCH

1. **Vô hiệu hóa nhanh một cấu hình**:
   * Đang stress test muốn chuyển sang chạy test tay: đổi `// [GEN: ...]` thành `// [-GEN: ...]`.
   * Muốn đổi Time Limit tạm thời: `// [TL: 0.5]`.
2. **Lưu test sai để nộp bài hoặc tạo testcase**:
   * Thêm tag `// [SAVE: "wa_tests"]`. Khi có test bị WA, file input và output sẽ được lưu ngay vào folder `wa_tests/`.
3. **Chỉ gộp code (Bundle Only)**:
   * Khi code xong và muốn xuất file nộp lên Codeforces / VNOI: thêm tag `// [BUNDLE-ONLY]`. Runner sẽ tạo ngay file `submit/submit.cpp` đã inline đầy đủ thư viện, xóa comment thừa, xóa lời gọi `debug(...)` và thay thế emoji.
