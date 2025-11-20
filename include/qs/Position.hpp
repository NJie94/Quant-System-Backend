#pragma once

#include <string>
#include <cstdlib>
#include "qs/Trade.hpp"

namespace qs
{

    struct Position
    {
        std::string symbol;
        int quantity{0};
        double avgPrice{0.0};

        void applyTrade(const Trade &t)
        {
            if (quantity == 0)
            {
                quantity = (t.side == Side::Buy ? t.quantity : -t.quantity);
                avgPrice = t.price;
                return;
            }

            int signedQty = (t.side == Side::Buy ? t.quantity : -t.quantity);
            int newQty = quantity + signedQty;

            if ((quantity > 0 && signedQty > 0) || (quantity < 0 && signedQty < 0))
            {
                // increasing existing direction
                double totalCost = avgPrice * std::abs(quantity) + t.price * std::abs(signedQty);
                int totalQty = std::abs(quantity) + std::abs(signedQty);
                avgPrice = totalCost / static_cast<double>(totalQty);
                quantity = newQty;
            }
            else
            {
                // reducing or flipping
                quantity = newQty;
                if (quantity == 0)
                {
                    avgPrice = 0.0;
                }
                else
                {
                    // flipped direction; reset avgPrice to last trade
                    avgPrice = t.price;
                }
            }
        }
    };

} // namespace qs
