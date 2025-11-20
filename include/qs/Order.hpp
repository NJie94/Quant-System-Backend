#pragma once

#include <string>
#include <chrono>
#include "qs/Bar.hpp"

namespace qs
{

    enum class Side
    {
        Buy,
        Sell
    };

    struct Order
    {
        std::string symbol;
        Side side{Side::Buy};
        int quantity{0};
        double limitPrice; // use NaN as "market"
        Date date{};
    };

} // namespace qs
