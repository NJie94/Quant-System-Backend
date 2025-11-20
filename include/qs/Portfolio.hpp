#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include "qs/Position.hpp"

namespace qs
{

    class Portfolio
    {
    public:
        explicit Portfolio(double startingCash)
            : _cash(startingCash), _equity(startingCash) {}

        void applyTrade(const Trade &t)
        {
            double signedQty = (t.side == Side::Buy ? t.quantity : -t.quantity);
            double cashChange = -signedQty * t.price - t.commission - t.slippage;
            _cash += cashChange;

            auto &pos = _positions[t.symbol];
            pos.symbol = t.symbol;
            pos.applyTrade(t);
        }

        template <typename PriceGetter>
        void markToMarket(PriceGetter getPrice)
        {
            double total = _cash;
            for (const auto &kv : _positions)
            {
                const auto &sym = kv.first;
                const auto &pos = kv.second;
                double price = getPrice(sym);
                total += pos.quantity * price;
            }
            _equity = total;
        }

        double cash() const noexcept { return _cash; }
        double equity() const noexcept { return _equity; }

        int positionQty(const std::string &symbol) const
        {
            auto it = _positions.find(symbol);
            if (it == _positions.end())
                return 0;
            return it->second.quantity;
        }

        const std::unordered_map<std::string, Position> &positions() const noexcept
        {
            return _positions;
        }

    private:
        double _cash{0.0};
        double _equity{0.0};
        std::unordered_map<std::string, Position> _positions;
    };

} // namespace qs
