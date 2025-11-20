#include "qs/Strategy.hpp"

#include <deque>
#include <unordered_map>
#include <algorithm>
#include <limits>

namespace qs
{

    namespace
    {

        struct SymbolState
        {
            std::deque<double> prices;
        };

        class MomentumStrategyImpl : public Strategy
        {
        public:
            MomentumStrategyImpl(int shortWindow,
                                 int longWindow,
                                 int unitSize)
                : _short(shortWindow),
                  _long(longWindow),
                  _unitSize(unitSize) {}

            std::vector<Order> onBar(const std::string &symbol,
                                     const Bar &bar,
                                     const StrategyContext &ctx) override
            {
                auto &state = _state[symbol];
                state.prices.push_back(bar.close);
                if (state.prices.size() > static_cast<std::size_t>(_long))
                {
                    state.prices.pop_front();
                }

                if (state.prices.size() < static_cast<std::size_t>(_long))
                {
                    return {};
                }

                double shortMA = avgLast(state.prices, _short);
                double longMA = avgLast(state.prices, _long);

                int pos = ctx.getPositionQty(symbol);
                std::vector<Order> orders;

                if (shortMA > longMA && pos <= 0)
                {
                    // bullish crossover -> go long
                    if (pos < 0)
                    {
                        orders.push_back(Order{symbol, Side::Buy, -pos,
                                               std::numeric_limits<double>::quiet_NaN(),
                                               bar.date});
                    }
                    orders.push_back(Order{symbol, Side::Buy, _unitSize,
                                           std::numeric_limits<double>::quiet_NaN(),
                                           bar.date});
                }
                else if (shortMA < longMA && pos >= 0)
                {
                    // bearish crossover -> exit longs
                    if (pos > 0)
                    {
                        orders.push_back(Order{symbol, Side::Sell, pos,
                                               std::numeric_limits<double>::quiet_NaN(),
                                               bar.date});
                    }
                }

                return orders;
            }

        private:
            int _short;
            int _long;
            int _unitSize;

            std::unordered_map<std::string, SymbolState> _state;

            static double avgLast(const std::deque<double> &d, int n)
            {
                n = std::min(n, static_cast<int>(d.size()));
                double sum = 0.0;
                for (int i = static_cast<int>(d.size()) - n; i < static_cast<int>(d.size()); ++i)
                {
                    sum += d[static_cast<std::size_t>(i)];
                }
                return sum / static_cast<double>(n);
            }
        };

    } // namespace

    std::unique_ptr<Strategy> makeMomentumStrategy(
        int shortWindow,
        int longWindow,
        int unitSize)
    {
        return std::make_unique<MomentumStrategyImpl>(
            shortWindow, longWindow, unitSize);
    }

} // namespace qs
