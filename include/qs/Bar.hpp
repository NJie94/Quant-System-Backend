#pragma once
#include <chrono>

namespace qs
{
    using Date = std::chrono::sys_days;

    struct Bar
    {
        Date date{};
        double open{0.0};
        double high{0.0};
        double low{0.0};
        double close{0.0};
        double volume{0.0};
    };

} // namespace qs
