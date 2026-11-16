#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <iomanip>
#include "../config.hpp"
#include "../style.hpp"
#include "../process.hpp"
#include "../diff.hpp"
#include "../tag_parser.hpp"
#include "../temp.hpp"

#ifndef CP_INIT_RUNNER_MODES_BATCH_HPP
#define CP_INIT_RUNNER_MODES_BATCH_HPP

namespace cp {
namespace init {
namespace runner {
namespace modes {

namespace fs = std::filesystem;

inline void save_failed_test(const fs::path& save_dir, const std::string& input, const std::string& actual, const std::string& expected, const std::string& error) {
    if (!config::save_failed) return;
    std::error_code ec;
    fs::create_directories(save_dir, ec);

    fs::path f_in = save_dir / "_failed.in";
    fs::path f_out = save_dir / "_failed.out";
    fs::path f_ans = save_dir / "_failed.ans";
    fs::path f_err = save_dir / "_failed.err";

    { std::ofstream f(f_in); f << input; }
    { std::ofstream f(f_out); f << actual; }
    if (!expected.empty()) { std::ofstream f(f_ans); f << expected; }
    if (!error.empty()) { std::ofstream f(f_err); f << error; }

    std::string prefix = save_dir.filename().string();
    std::cout << style::color_yellow << "saved failed test to: " << save_dir.string() << style::reset << "\n";
}

inline fs::path resolve_save_dir(const fs::path& src_dir) {
    if (!config::failed_save_dir.empty()) {
        fs::path p(config::failed_save_dir);
        if (p.is_absolute()) return p;
        return src_dir / p;
    }
    return src_dir;
}

inline fs::path get_save_dir(const fs::path& src_dir) {
    return resolve_save_dir(src_dir);
}

inline int run_manual(const fs::path& sol_exe) {
    std::cout << style::color_white << "mode: " << style::color_green << "manual" << style::reset << "\n";
    std::string cmd = "\"" + sol_exe.string() + "\"";
    int code = std::system(cmd.c_str());
#ifndef _WIN32
    if (WIFEXITED(code)) code = WEXITSTATUS(code);
#endif
    return code;
}

inline int run_batch(const fs::path& sol_exe,
                     const fs::path& src_dir,
                     const std::vector<test_case>& tests,
                     const fs::path& checker_exe = "",
                     const std::string& checker_name = "",
                     bool force_run_only = false) {
    bool has_checker = !checker_exe.empty() && !force_run_only;
    bool has_any_expected = false;
    if (!force_run_only) {
        for (const auto& tc : tests) {
            if (!tc.skipped && tc.has_expected) {
                has_any_expected = true;
                break;
            }
        }
    }

    std::string mode_str = has_checker ? "checker" : (has_any_expected ? "evaluate" : "run only");
    std::cout << style::color_white << "mode: " << style::color_green << mode_str << style::reset << "\n";
    if (has_checker) {
        std::cout << style::color_white << "checker: " << style::color_cyan << checker_name << style::reset << "\n";
    }

    int passed = 0;
    int failed = 0;
    int total_run = 0;

    for (size_t i = 0; i < tests.size(); ++i) {
        const auto& tc = tests[i];
        if (tc.skipped) {
            std::cout << style::color_white << "test " << style::color_blue << (i + 1) << style::color_white << ": "
                      << style::color_yellow << "skipped" << style::reset << "\n";
            continue;
        }

        total_run++;
        int test_num = static_cast<int>(i + 1);

        run_result sol_res = run_with_timeout(sol_exe.string(), tc.input, config::time_limit);

        bool is_evaluate = !force_run_only && (has_checker || tc.has_expected);

        if (!is_evaluate) {
            // Chế độ run only (chạy và in output toàn bộ các test)
            if (sol_res.is_timeout) {
                failed++;
                std::cout << style::color_white << "test " << style::color_blue << test_num << style::color_white << ": "
                          << style::color_red << "tle " << style::color_white << "[" << style::color_yellow << "> " << std::fixed << std::setprecision(3) << config::time_limit << "s" << style::color_white << "]" << style::reset << "\n";
            } else if (sol_res.exit_code != 0) {
                failed++;
                std::cout << style::color_white << "test " << style::color_blue << test_num << style::color_white << ": "
                          << style::color_red << "rte " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            } else {
                passed++;
                std::cout << style::color_white << "test " << style::color_blue << test_num << style::color_white << ": "
                          << style::color_green << "ok" << style::reset << "\n";
            }

            std::cout << style::color_black << "input:" << style::reset << "\n";
            std::cout << style::color_white << tc.input << style::reset << (tc.input.empty() || tc.input.back() == '\n' ? "" : "\n");

            if (config::show_debug && !sol_res.error_output.empty()) {
                std::cout << style::color_black << "debug:" << style::reset << "\n";
                std::cout << style::color_yellow << sol_res.error_output << style::reset << (sol_res.error_output.back() == '\n' ? "" : "\n");
            }

            if (!sol_res.output.empty()) {
                std::cout << style::color_black << "output:" << style::reset << "\n";
                std::cout << "\x1b[36m" << sol_res.output << style::reset << (sol_res.output.back() == '\n' ? "" : "\n");
            }

            std::cout << style::color_black << "done: " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            continue;
        }

        // Chế độ evaluate hoặc checker
        bool is_ac = false;
        std::string fail_msg = "";
        run_result chk_res;

        if (sol_res.is_timeout) {
            std::cout << style::color_white << "test " << style::color_blue << test_num << style::color_white << ": "
                      << style::color_red << "tle " << style::color_white << "[" << style::color_yellow << "> " << std::fixed << std::setprecision(3) << config::time_limit << "s" << style::color_white << "]" << style::reset << "\n";
        } else if (sol_res.exit_code != 0) {
            std::cout << style::color_white << "test " << style::color_blue << test_num << style::color_white << ": "
                      << style::color_red << "rte " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
        } else if (has_checker) {
            // Chạy checker <in> <out> <ans>
            fs::path tmp_in = temp_session::instance().temp_file("chk_in", ".in");
            fs::path tmp_out = temp_session::instance().temp_file("chk_out", ".out");
            fs::path tmp_ans = temp_session::instance().temp_file("chk_ans", ".ans");

            { std::ofstream f(tmp_in); f << tc.input; }
            { std::ofstream f(tmp_out); f << sol_res.output; }
            { std::ofstream f(tmp_ans); f << tc.expected; }

            std::vector<std::string> chk_args = {
                tmp_in.string(),
                tmp_out.string(),
                tmp_ans.string()
            };
            chk_res = run_with_timeout(checker_exe.string(), chk_args, "", config::time_limit);

            if (chk_res.exit_code == 0 && !chk_res.is_timeout) {
                is_ac = true;
            } else {
                fail_msg = chk_res.error_output;
            }
        } else {
            // evaluate với token match + float epsilon
            if (diff::check_match(sol_res.output, tc.expected)) {
                is_ac = true;
            }
        }

        if (is_ac) {
            std::cout << style::color_white << "test " << style::color_blue << test_num << style::color_white << ": "
                      << style::color_green << "ac " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            passed++;
        } else {
            failed++;
            if (!sol_res.is_timeout && sol_res.exit_code == 0) {
                std::cout << style::color_white << "test " << style::color_blue << test_num << style::color_white << ": "
                          << style::color_red << "wa " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            }

            std::cout << style::color_black << "input:" << style::reset << "\n";
            std::cout << style::color_white << tc.input << style::reset << (tc.input.empty() || tc.input.back() == '\n' ? "" : "\n");

            if (config::show_debug && !sol_res.error_output.empty()) {
                std::cout << style::color_black << (has_checker ? "sol debug:" : "debug:") << style::reset << "\n";
                std::cout << style::color_yellow << sol_res.error_output << style::reset << (sol_res.error_output.back() == '\n' ? "" : "\n");
            }

            if (has_checker) {
                if (config::show_debug && !chk_res.error_output.empty()) {
                    std::cout << style::color_black << "checker debug:" << style::reset << "\n";
                    std::cout << style::color_yellow << chk_res.error_output << style::reset << (chk_res.error_output.back() == '\n' ? "" : "\n");
                }
                if (!chk_res.output.empty()) {
                    std::cout << style::color_black << "checker message:" << style::reset << "\n";
                    std::cout << style::color_yellow << chk_res.output << style::reset << (chk_res.output.back() == '\n' ? "" : "\n");
                }
                if (chk_res.is_timeout) {
                    std::cout << style::color_red << "checker timeout (> " << std::fixed << std::setprecision(3) << config::time_limit << "s)" << style::reset << "\n";
                } else if (chk_res.exit_code != 0 && chk_res.exit_code != 1) {
                    std::cout << style::color_red << "checker runtime error (exit code " << chk_res.exit_code << ")" << style::reset << "\n";
                }
            } else if (!fail_msg.empty()) {
                std::cout << style::color_yellow << fail_msg << style::reset << (fail_msg.back() == '\n' ? "" : "\n");
            }

            if (sol_res.is_timeout || sol_res.exit_code != 0) {
                if (!sol_res.output.empty()) {
                    std::cout << style::color_black << "actual:" << style::reset << "\n";
                    std::cout << sol_res.output << (sol_res.output.back() == '\n' ? "" : "\n");
                }
            } else if (!has_checker) {
                diff::print_diff(sol_res.output, tc.expected);
            } else {
                if (!sol_res.output.empty()) {
                    std::cout << style::color_black << "actual:" << style::reset << "\n";
                    std::cout << sol_res.output << (sol_res.output.back() == '\n' ? "" : "\n");
                }
            }

            // Lưu test lỗi nếu cấu hình bật
            if (config::save_failed) {
                save_failed_test(resolve_save_dir(src_dir), tc.input, sol_res.output, tc.expected, sol_res.error_output);
            }
        }
    }

    if (total_run > 0) {
        if (passed == total_run) {
            if (total_run > 1) {
                std::cout << style::color_green << "all " << total_run << " tests passed!" << style::reset << "\n";
            } else {
                std::cout << style::color_green << "all tests passed!" << style::reset << "\n";
            }
            return 0;
        } else {
            std::cout << style::color_red << "failed " << failed << "/" << total_run << " tests!" << style::reset << "\n";
            return 1;
        }
    }
    return 0;
}

} // namespace modes
} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_MODES_BATCH_HPP