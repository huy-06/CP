#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <iomanip>
#include "../config.hpp"
#include "../style.hpp"
#include "../process.hpp"
#include "../diff.hpp"
#include "../temp.hpp"
#include "batch.hpp"

#ifndef CP_INIT_RUNNER_MODES_STRESS_HPP
#define CP_INIT_RUNNER_MODES_STRESS_HPP

namespace cp {
namespace init {
namespace runner {
namespace modes {

namespace fs = std::filesystem;

inline void append_failed_to_src_file(const fs::path& src_file, const std::string& input, const std::string& expected) {
    if (!config::append_failed_to_src) return;
    std::ifstream fin(src_file, std::ios::binary);
    if (!fin) return;
    std::string content((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
    fin.close();

    std::string trimmed_in = detail::trim(input);
    if (trimmed_in.empty()) return;

    // Không chèn nếu test case này đã có trong file
    if (content.find(trimmed_in) != std::string::npos) return;

    size_t last_block_end = content.rfind("*/");
    if (last_block_end != std::string::npos) {
        size_t insert_pos = last_block_end;
        while (insert_pos > 0 && (content[insert_pos - 1] == ' ' || content[insert_pos - 1] == '\t')) {
            --insert_pos;
        }
        std::string test_block = "";
        if (insert_pos > 0 && content[insert_pos - 1] != '\n') {
            test_block += "\n";
        }
        test_block += "[IN]\n" + trimmed_in + "\n";
        if (!expected.empty()) {
            test_block += "[OUT]\n" + detail::trim(expected) + "\n";
        }
        content.insert(insert_pos, test_block);
    } else {
        std::string test_block = "[IN]\n" + trimmed_in + "\n";
        if (!expected.empty()) {
            test_block += "[OUT]\n" + detail::trim(expected) + "\n";
        }
        content += "\n/*\n" + test_block + "*/\n";
    }

    std::ofstream fout(src_file, std::ios::binary | std::ios::trunc);
    if (fout) {
        fout << content;
        std::cout << style::color_green << "appended [IN] and [OUT] to " << src_file.filename().string() << style::reset << "\n";
    }
}

inline int run_stress(const fs::path& sol_exe,
                      const fs::path& gen_exe,
                      const std::string& gen_name,
                      const fs::path& brute_exe,
                      const std::string& brute_name,
                      const fs::path& checker_exe,
                      const std::string& checker_name,
                      int max_iters,
                      const fs::path& src_dir,
                      const fs::path& src_file = "") {
    bool has_brute = !brute_exe.empty();
    bool has_checker = !checker_exe.empty();

    std::string sub_mode = has_checker ? "[checker]" : (has_brute ? "[evaluate]" : "[run only]");
    std::cout << style::color_white << "mode: " << style::color_green << "stress " << sub_mode << style::reset << "\n";
    std::cout << style::color_white << "generator: " << style::color_cyan << gen_name << style::reset << "\n";
    if (has_brute) {
        std::cout << style::color_white << "brute: " << style::color_cyan << brute_name << style::reset << "\n";
    }
    if (has_checker) {
        std::cout << style::color_white << "checker: " << style::color_cyan << checker_name << style::reset << "\n";
    }

    fs::path save_dir = get_save_dir(src_dir);
    int iter = 1;

    while (max_iters <= 0 || iter <= max_iters) {
        // Sinh test từ gen
        run_result gen_res = run_with_timeout(gen_exe.string(), std::to_string(iter) + "\n", config::time_limit);
        if (gen_res.is_timeout || gen_res.exit_code != 0) {
            std::cout << style::color_red << "generator error at test " << iter << style::reset << "\n";
            if (!gen_res.error_output.empty()) {
                std::cout << style::color_black << "generator debug:" << style::reset << "\n";
                std::cout << style::color_yellow << gen_res.error_output << style::reset << (gen_res.error_output.back() == '\n' ? "" : "\n");
            }
            return 1;
        }

        std::string current_in = gen_res.output;
        run_result sol_res = run_with_timeout(sol_exe.string(), current_in, config::time_limit);

        if (sol_res.is_timeout) {
            std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                      << style::color_red << "tle " << style::color_white << "[" << style::color_yellow << "> " << std::fixed << std::setprecision(3) << config::time_limit << "s" << style::color_white << "]" << style::reset << "\n";
            save_failed_test(save_dir, current_in, sol_res.output, "", sol_res.error_output);
            if (!src_file.empty()) append_failed_to_src_file(src_file, current_in, "");
            std::cout << style::color_black << "input:" << style::reset << "\n";
            std::cout << style::color_white << current_in << style::reset << (current_in.empty() || current_in.back() == '\n' ? "" : "\n");
            if (!sol_res.error_output.empty() && config::show_debug) {
                std::cout << style::color_black << "sol debug:" << style::reset << "\n";
                std::cout << style::color_yellow << sol_res.error_output << style::reset << (sol_res.error_output.back() == '\n' ? "" : "\n");
            }
            return 1;
        }

        if (sol_res.exit_code != 0) {
            std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                      << style::color_red << "rte " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            save_failed_test(save_dir, current_in, sol_res.output, "", sol_res.error_output);
            if (!src_file.empty()) append_failed_to_src_file(src_file, current_in, "");
            std::cout << style::color_black << "input:" << style::reset << "\n";
            std::cout << style::color_white << current_in << style::reset << (current_in.empty() || current_in.back() == '\n' ? "" : "\n");
            if (!sol_res.error_output.empty() && config::show_debug) {
                std::cout << style::color_black << "sol debug:" << style::reset << "\n";
                std::cout << style::color_yellow << sol_res.error_output << style::reset << (sol_res.error_output.back() == '\n' ? "" : "\n");
            }
            return 1;
        }

        if (has_brute) {
            run_result brute_res = run_with_timeout(brute_exe.string(), current_in, config::time_limit);
            if (brute_res.is_timeout || brute_res.exit_code != 0) {
                std::cout << style::color_red << "brute error at test " << iter << style::reset << "\n";
                if (!brute_res.error_output.empty()) {
                    std::cout << style::color_black << "brute debug:" << style::reset << "\n";
                    std::cout << style::color_yellow << brute_res.error_output << style::reset << (brute_res.error_output.back() == '\n' ? "" : "\n");
                }
                return 1;
            }

            if (has_checker) {
                // Có cả brute lẫn checker: checker kiểm tra sol và brute
                fs::path tmp_in = temp_session::instance().temp_file("chk_in", ".in");
                fs::path tmp_out = temp_session::instance().temp_file("chk_out", ".out");
                fs::path tmp_ans = temp_session::instance().temp_file("chk_ans", ".ans");

                { std::ofstream f(tmp_in); f << current_in; }
                { std::ofstream f(tmp_out); f << sol_res.output; }
                { std::ofstream f(tmp_ans); f << brute_res.output; }

                std::vector<std::string> chk_args = { tmp_in.string(), tmp_out.string(), tmp_ans.string() };
                run_result chk_res = run_with_timeout(checker_exe.string(), chk_args, "", config::time_limit);

                if (chk_res.exit_code != 0 || chk_res.is_timeout) {
                    std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                              << style::color_red << "wa " << style::color_white << "[sol: " << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s"
                              << style::color_white << ", brute: " << style::color_yellow << brute_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
                    save_failed_test(save_dir, current_in, sol_res.output, brute_res.output, chk_res.error_output);
                    if (!src_file.empty()) append_failed_to_src_file(src_file, current_in, brute_res.output);

                    std::cout << style::color_black << "input:" << style::reset << "\n";
                    std::cout << style::color_white << current_in << style::reset << (current_in.empty() || current_in.back() == '\n' ? "" : "\n");

                    if (config::show_debug && !sol_res.error_output.empty()) {
                        std::cout << style::color_black << "sol debug:" << style::reset << "\n";
                        std::cout << style::color_yellow << sol_res.error_output << style::reset << (sol_res.error_output.back() == '\n' ? "" : "\n");
                    }
                    if (config::show_debug && !brute_res.error_output.empty()) {
                        std::cout << style::color_black << "brute debug:" << style::reset << "\n";
                        std::cout << style::color_yellow << brute_res.error_output << style::reset << (brute_res.error_output.back() == '\n' ? "" : "\n");
                    }
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
                    if (!sol_res.output.empty()) {
                        std::cout << style::color_black << "actual:" << style::reset << "\n";
                        std::cout << sol_res.output << (sol_res.output.back() == '\n' ? "" : "\n");
                    }
                    if (!brute_res.output.empty()) {
                        std::cout << style::color_black << "expected:" << style::reset << "\n";
                        std::cout << brute_res.output << (brute_res.output.back() == '\n' ? "" : "\n");
                    }
                    return 1;
                } else {
                    std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                              << style::color_green << "ac " << style::color_white << "[sol: " << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s"
                              << style::color_white << ", brute: " << style::color_yellow << brute_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
                }
            } else {
                // Token diff
                if (!diff::check_match(sol_res.output, brute_res.output)) {
                    std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                              << style::color_red << "wa " << style::color_white << "[sol: " << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s"
                              << style::color_white << ", brute: " << style::color_yellow << brute_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
                    save_failed_test(save_dir, current_in, sol_res.output, brute_res.output, sol_res.error_output);
                    if (!src_file.empty()) append_failed_to_src_file(src_file, current_in, brute_res.output);

                    std::cout << style::color_black << "input:" << style::reset << "\n";
                    std::cout << style::color_white << current_in << style::reset << (current_in.empty() || current_in.back() == '\n' ? "" : "\n");

                    if (config::show_debug && !sol_res.error_output.empty()) {
                        std::cout << style::color_black << "sol debug:" << style::reset << "\n";
                        std::cout << style::color_yellow << sol_res.error_output << style::reset << (sol_res.error_output.back() == '\n' ? "" : "\n");
                    }
                    if (config::show_debug && !brute_res.error_output.empty()) {
                        std::cout << style::color_black << "brute debug:" << style::reset << "\n";
                        std::cout << style::color_yellow << brute_res.error_output << style::reset << (brute_res.error_output.back() == '\n' ? "" : "\n");
                    }
                    diff::print_diff(sol_res.output, brute_res.output);
                    return 1;
                } else {
                    std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                              << style::color_green << "ac " << style::color_white << "[sol: " << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s"
                              << style::color_white << ", brute: " << style::color_yellow << brute_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
                }
            }
        } else if (has_checker) {
            // Chỉ có checker, không có brute (bài checker tự kiểm tra sol qua input)
            fs::path tmp_in = temp_session::instance().temp_file("chk_in", ".in");
            fs::path tmp_out = temp_session::instance().temp_file("chk_out", ".out");
            fs::path tmp_ans = temp_session::instance().temp_file("chk_ans", ".ans");

            { std::ofstream f(tmp_in); f << current_in; }
            { std::ofstream f(tmp_out); f << sol_res.output; }
            { std::ofstream f(tmp_ans); f << ""; }

            std::vector<std::string> chk_args = { tmp_in.string(), tmp_out.string(), tmp_ans.string() };
            run_result chk_res = run_with_timeout(checker_exe.string(), chk_args, "", config::time_limit);

            if (chk_res.exit_code != 0 || chk_res.is_timeout) {
                std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                          << style::color_red << "wa " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
                save_failed_test(save_dir, current_in, sol_res.output, "", chk_res.error_output);
                if (!src_file.empty()) append_failed_to_src_file(src_file, current_in, "");

                std::cout << style::color_black << "input:" << style::reset << "\n";
                std::cout << style::color_white << current_in << style::reset << (current_in.empty() || current_in.back() == '\n' ? "" : "\n");

                if (config::show_debug && !sol_res.error_output.empty()) {
                    std::cout << style::color_black << "sol debug:" << style::reset << "\n";
                    std::cout << style::color_yellow << sol_res.error_output << style::reset << (sol_res.error_output.back() == '\n' ? "" : "\n");
                }
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
                if (!sol_res.output.empty()) {
                    std::cout << style::color_black << "actual:" << style::reset << "\n";
                    std::cout << sol_res.output << (sol_res.output.back() == '\n' ? "" : "\n");
                }
                return 1;
            } else {
                std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                          << style::color_green << "ac " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            }
        } else {
            // Run only mode (chỉ kiểm tra crash / TLE) -> in "ok"
            std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                      << style::color_green << "ok " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << sol_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
        }

        iter++;
    }

    if (max_iters > 1) {
        std::cout << style::color_green << "all " << (iter - 1) << " tests passed!" << style::reset << "\n";
    }
    return 0;
}

} // namespace modes
} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_MODES_STRESS_HPP
