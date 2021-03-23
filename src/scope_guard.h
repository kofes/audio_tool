#pragma once

#include <functional>
#include <exception>

namespace custom {
class scope_guard {
public:
    template<typename Fn>
    scope_guard(Fn&& fn):
        cleanup_(std::forward<Fn>(fn)) {}

    ~scope_guard() {
        if (std::uncaught_exception() && cleanup_) {
            cleanup_();
        }
    }

protected:
    std::function<void()> cleanup_;
};
} // namespace custom
