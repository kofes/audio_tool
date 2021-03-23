#pragma once

#include <iostream>

namespace log {
    static auto&& error_out = std::cerr;
    static auto&& warning_out = std::cout;

    auto&& error() noexcept {
        return (error_out << "[ERROR]: ");
    }

    auto&& warning() noexcept {
        return (warning_out << "[WARNING]: ");
    }
}