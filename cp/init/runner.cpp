// RUNNER & BUNDLER CHO COMPETITIVE PROGRAMMING (CROSS-PLATFORM: WINDOWS / LINUX)
// Biên dịch:
//   - Linux / WSL: g++ -std=c++23 -O2 runner.cpp -o runner
//   - Windows:     g++ -std=c++23 -O2 runner.cpp -o runner.exe
// Sử dụng:
//   ./runner <source_file.cpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <thread>
#include <csignal>
#include <cstdlib>

#include "runner_modules/config.hpp"
#include "runner_modules/style.hpp"
#include "runner_modules/platform.hpp"
#include "runner_modules/temp.hpp"
#include "runner_modules/process.hpp"
#include "runner_modules/compiler.hpp"
#include "runner_modules/tag_parser.hpp"
#include "runner_modules/bundler.hpp"
#include "runner_modules/diff.hpp"
#include "runner_modules/modes/batch.hpp"
#include "runner_modules/modes/stress.hpp"
#include "runner_modules/modes/interactive.hpp"
#include "runner_modules/modes/judge_mode.hpp"

using namespace cp::init::runner;

namespace fs = std::filesystem;

inline fs::path normalize_path(const std::string& raw_path) {
    std::string s = raw_path;
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        s = s.substr(1, s.size() - 2);
    }
    std::error_code ec;
    return fs::absolute(s, ec).lexically_normal();
}

int main(int argc, char* argv[]) {
    platform::enable_vt_processing();

    if (argc < 2) {
        std::cerr << "usage: runner <source_file.cpp>\n";
        return 1;
    }

    fs::path src_file = normalize_path(argv[1]);
    fs::path src_dir = src_file.parent_path();
    fs::path runner_dir = normalize_path(argv[0]).parent_path();

    std::string src_content = read_file_content(src_file);
    if (src_content.empty()) {
        std::error_code ec;
        if (!fs::exists(src_file, ec)) {
            std::cerr << style::color_red << "error: could not open source file: " << src_file.string() << style::reset << "\n";
            return 1;
        }
    }

    // Parse comment tags và trích xuất tests inline [IN]/[OUT] hoặc file/thư mục
    parse_output parsed = parse_source(src_content, src_dir);

    // Báo lỗi nếu có file hoặc thư mục test không tìm thấy
    if (!parsed.errors.empty()) {
        for (const auto& err : parsed.errors) {
            std::cerr << style::color_red << "error: " << err << style::reset << "\n";
        }
        return 1;
    }

    // Xử lý chế độ chỉ bundle file (bundle-only)
    if (config::bundle_only) {
        fs::path submit_file = bundler::resolve_submit_path(src_file);
        if (bundler::bundle_to(src_file, submit_file)) {
            std::cout << style::color_green << "bundled: " << submit_file.string() << style::reset << "\n";
            return 0;
        } else {
            std::cerr << style::color_red << "bundle failed: " << submit_file.string() << style::reset << "\n";
            return 1;
        }
    }

    // Chạy bundler nền nếu bật
    std::thread bundle_th([&]() {
        try {
            bundler::run(src_file);
        } catch (...) {}
    });

    // Temp session quản lý toàn bộ exe biên dịch ra trong temp OS
    temp_session& tmp = temp_session::instance();

    // Kiểm tra các mode đặc biệt
    bool has_itor = parsed.tags.interactive.found && !parsed.tags.interactive.skipped;
    bool has_judge = parsed.tags.judge.found && !parsed.tags.judge.skipped;
    bool has_gen = parsed.tags.gen.found && !parsed.tags.gen.skipped;
    bool has_brute = parsed.tags.brute.found && !parsed.tags.brute.skipped;
    bool has_checker = parsed.tags.checker.found && !parsed.tags.checker.skipped;

    // Biên dịch solution
    fs::path sol_exe = tmp.exe("sol");
    fs::path checker_exe;
    std::string checker_name;

    std::vector<std::pair<fs::path, fs::path>> to_compile = {
        {src_file, sol_exe}
    };

    if (has_checker) {
        fs::path checker_src = resolve_relative_path(src_dir, parsed.tags.checker.value);
        if (!fs::exists(checker_src)) {
            std::cerr << style::color_red << "error: checker file not found: " << checker_src.string() << style::reset << "\n";
            if (bundle_th.joinable()) bundle_th.join();
            return 1;
        }
        checker_exe = tmp.exe("checker");
        checker_name = checker_src.filename().string();
        to_compile.push_back({checker_src, checker_exe});
    }

    auto comp_results = compile_parallel(to_compile);
    for (const auto& cr : comp_results) {
        if (!cr.ok) {
            if (bundle_th.joinable()) bundle_th.join();
            return 1;
        }
    }

    // Kiểm tra mode override ([MANUAL], [RUN ONLY], [EVALUATE])
    bool force_manual = (parsed.tags.mode_override == "manual");
    bool force_run_only = (parsed.tags.mode_override == "run_only");
    bool force_evaluate = (parsed.tags.mode_override == "evaluate");

    // Nếu chọn mode MANUAL: chạy tương tác trực tiếp
    if (force_manual) {
        int code = modes::run_manual(sol_exe);
        if (bundle_th.joinable()) bundle_th.join();
        return code;
    }

    // Nếu ép [RUN ONLY] hoặc [EVALUATE] nhưng không có test [IN] nào
    if ((force_run_only || force_evaluate) && parsed.tests.empty() && !has_itor && !has_judge && !has_gen) {
        std::cerr << style::color_red << "error: [" << parsed.tags.mode_override_tag << "] mode specified but no [IN] test cases found." << style::reset << "\n";
        if (bundle_th.joinable()) bundle_th.join();
        return 1;
    }

    // 1. Chạy các test case tĩnh [IN] / [OUT] hoặc [FILE_IN] trước nếu có
    if (!parsed.tests.empty()) {
        int code = modes::run_batch(sol_exe, src_dir, parsed.tests, checker_exe, checker_name, force_run_only);
        if (code != 0) {
            // Test tĩnh bị lỗi -> dừng ngay để sửa
            if (bundle_th.joinable()) bundle_th.join();
            return code;
        }
        // Nếu không có mode mở rộng nào khác -> hoàn tất
        if (!has_itor && !has_judge && !has_gen) {
            if (bundle_th.joinable()) bundle_th.join();
            return 0;
        }
        std::cout << "\n";
    }

    // 2. Chạy mode INTERACTIVE nếu có
    if (has_itor) {
        std::string itor_file_str;
        int itor_iters = 1;
        bool tag_no_log = false;
        parse_tag_file_and_iters(parsed.tags.interactive.value, itor_file_str, itor_iters, &tag_no_log);
        if (tag_no_log) config::show_log = false;

        fs::path itor_src = resolve_relative_path(src_dir, itor_file_str);
        if (!fs::exists(itor_src)) {
            std::cerr << style::color_red << "error: interactor file not found: " << itor_src.string() << style::reset << "\n";
            if (bundle_th.joinable()) bundle_th.join();
            return 1;
        }

        fs::path itor_exe = tmp.exe("itor");
        compile_result cr_itor = compile_target(itor_src, itor_exe);
        if (!cr_itor.ok) {
            if (bundle_th.joinable()) bundle_th.join();
            return 1;
        }

        int code = modes::run_interactive(sol_exe, itor_exe, itor_src.filename().string(), itor_iters);
        if (bundle_th.joinable()) bundle_th.join();
        return code;
    }

    // 3. Chạy mode JUDGE nếu có
    if (has_judge) {
        std::string judge_file_str;
        int judge_iters = 1;
        parse_tag_file_and_iters(parsed.tags.judge.value, judge_file_str, judge_iters);

        fs::path judge_src = resolve_relative_path(src_dir, judge_file_str);
        if (!fs::exists(judge_src)) {
            std::cerr << style::color_red << "error: judge file not found: " << judge_src.string() << style::reset << "\n";
            if (bundle_th.joinable()) bundle_th.join();
            return 1;
        }

        fs::path judge_exe = tmp.exe("judge");
        std::string judge_flags = "-I \"" + runner_dir.string() + "\"";
        compile_result cr_judge = compile_target(judge_src, judge_exe, judge_flags);
        if (!cr_judge.ok) {
            if (bundle_th.joinable()) bundle_th.join();
            return 1;
        }

        int code = modes::run_judge(sol_exe, judge_exe, judge_src.filename().string(), judge_iters);
        if (bundle_th.joinable()) bundle_th.join();
        return code;
    }

    // 4. Chạy mode STRESS (GEN) nếu có
    if (has_gen) {
        std::string gen_file_str;
        int gen_iters = 1;
        parse_tag_file_and_iters(parsed.tags.gen.value, gen_file_str, gen_iters);

        fs::path gen_src = resolve_relative_path(src_dir, gen_file_str);
        if (!fs::exists(gen_src)) {
            std::cerr << style::color_red << "error: generator file not found: " << gen_src.string() << style::reset << "\n";
            if (bundle_th.joinable()) bundle_th.join();
            return 1;
        }

        fs::path gen_exe = tmp.exe("gen");
        fs::path brute_exe;
        std::string brute_name;

        std::vector<std::pair<fs::path, fs::path>> stress_compile = {
            {gen_src, gen_exe}
        };

        if (has_brute) {
            fs::path brute_src = resolve_relative_path(src_dir, parsed.tags.brute.value);
            if (!fs::exists(brute_src)) {
                std::cerr << style::color_red << "error: brute file not found: " << brute_src.string() << style::reset << "\n";
                if (bundle_th.joinable()) bundle_th.join();
                return 1;
            }
            brute_exe = tmp.exe("brute");
            brute_name = brute_src.filename().string();
            stress_compile.push_back({brute_src, brute_exe});
        }

        auto s_results = compile_parallel(stress_compile);
        for (const auto& cr : s_results) {
            if (!cr.ok) {
                if (bundle_th.joinable()) bundle_th.join();
                return 1;
            }
        }

        int code = modes::run_stress(sol_exe, gen_exe, gen_src.filename().string(), brute_exe, brute_name, checker_exe, checker_name, gen_iters, src_dir, src_file);
        if (bundle_th.joinable()) bundle_th.join();
        return code;
    }

    // 5. Nếu hoàn toàn không có test case nào: chạy chế độ manual
    if (parsed.tests.empty()) {
        int code = modes::run_manual(sol_exe);
        if (bundle_th.joinable()) bundle_th.join();
        return code;
    }

    if (bundle_th.joinable()) bundle_th.join();
    return 0;
}
