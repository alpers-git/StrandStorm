#pragma once

#include <spdlog/spdlog.h>
#include <filesystem>

namespace fs = std::filesystem;

void setupLogging();

namespace spdlog {
    template <typename... Args>
    void assrt(bool condition, std::string_view fmt, Args&& ...args)
    {
        if (!condition) {
            spdlog::error(fmt::runtime(fmt), std::forward<Args>(args)...);
        }
    }
};