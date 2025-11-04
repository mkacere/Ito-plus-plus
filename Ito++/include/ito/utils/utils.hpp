#pragma once
#include <format>
#include <iostream>
#include <string_view>
#include <utility>

namespace ito::dbg {

    // ---- compile-time checked formatting (for literals) ----
    template<class... Args>
    inline void print(std::format_string<Args...> fmt, Args&&... args) {
        std::cout << std::format(fmt, std::forward<Args>(args)...);
    }

    template<class... Args>
    inline void println(std::format_string<Args...> fmt, Args&&... args) {
        std::cout << std::format(fmt, std::forward<Args>(args)...) << '\n';
    }

    // ---- runtime plain text (no formatting args) ----
    inline void print(std::string_view s) { std::cout << s; }
    inline void println(std::string_view s) { std::cout << s << '\n'; }

    // (Optional) if you need runtime *formatted* strings later, give it a new name:
    template<class... Args>
    inline void vprintln(std::string_view fmt, Args&&... args) {
        std::cout << std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)) << '\n';
    }

} // namespace ito::dbg