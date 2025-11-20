#pragma once
#include <deque>
#include <unordered_map>
#include "Strategy.hpp"

namespace qs
{
    class MeanReversionStrategy : public Strategy {
    public:
        MeanReversionStrategy(int shortWindow, int longWindow)
            : _short(shortWindow), _long(longWindow) {}

        std::vector<Order> onBar(const std::string& symbol,
                                const Bar& bar,
                                const StrategyContext& ctx) override {
            auto& state = _state[symbol];

            state.prices.push_back(bar.close);
            if (state.prices.size() > static_cast<size_t>(_long)) {
                state.prices.pop_front();
            }

            if (state.prices.size() < static_cast<size_t>(_long)) {
                return {};
            }

            double shortMA = avgLast(state.prices, _short);
            double longMA  = avgLast(state.prices, _long);

            std::vector<Order> orders;
            int pos = ctx.getPositionQty(symbol);

            if (shortMA > longMA && pos <= 0) {
                // go long
                orders.push_back(Order{symbol, Side::Buy, _unitSize, std::numeric_limits<double>::quiet_NaN(), bar.date});
            } else if (shortMA < longMA && pos >= 0) {
                // go flat (simple version: sell everything)
                if (pos > 0) {
                    orders.push_back(Order{symbol, Side::Sell, pos, std::numeric_limits<double>::quiet_NaN(), bar.date});
                }
            }

            return orders;
        }

    private:
        struct SymbolState {
            std::deque<double> prices;
        };

        int _short;
        int _long;
        int _unitSize{100};
        std::unordered_map<std::string, SymbolState> _state;

        static double avgLast(const std::deque<double>& d, int n) {
            n = std::min(n, static_cast<int>(d.size()));
            double sum = 0.0;
            for (int i = d.size() - n; i < static_cast<int>(d.size()); ++i) {
                sum += d[i];
            }
            return sum / n;
        }
    };
}