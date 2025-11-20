#include "qs/Strategy.hpp"

#include <deque>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace qs
{

    namespace
    {

        struct SymbolState
        {
            std::deque<double> prices;
        };

        class MeanReversionStrategyImpl : public Strategy
        {
        public:
            MeanReversionStrategyImpl(int lookbackWindow,
                                      double zEntry,
                                      double zExit,
                                      int unitSize)
                : _lookback(lookbackWindow),
                  _zEntry(zEntry),
                  _zExit(zExit),
                  _unitSize(unitSize) {}

            std::vector<Order> onBar(const std::string &symbol,
                                     const Bar &bar,
                                     const StrategyContext &ctx) override
            {
                auto &state = _state[symbol];
                state.prices.push_back(bar.close);
                if (state.prices.size() > static_cast<std::size_t>(_lookback))
                {
                    state.prices.pop_front();
                }

                if (state.prices.size() < static_cast<std::size_t>(_lookback))
                {
                    return {};
                }

                // compute mean & stddev
                double sum = 0.0;
                for (double p : state.prices)
                    sum += p;
                double mean = sum / static_cast<double>(state.prices.size());

                double var = 0.0;
                for (double p : state.prices)
                {
                    double diff = p - mean;
                    var += diff * diff;
                }
                var /= static_cast<double>(state.prices.size());
                double stddev = std::sqrt(var);
                if (stddev <= 0.0)
                    return {};

                double z = (bar.close - mean) / stddev;
                int pos = ctx.getPositionQty(symbol);

                std::vector<Order> orders;

                if (z < -_zEntry && pos <= 0)
                {
                    // price is too low: go long
                    orders.push_back(Order{symbol, Side::Buy, _unitSize,
                                           std::numeric_limits<double>::quiet_NaN(),
                                           bar.date});
                }
                else if (z > _zEntry && pos >= 0)
                {
                    // price is too high: go short (simple: just close longs here)
                    if (pos > 0)
                    {
                        orders.push_back(Order{symbol, Side::Sell, pos,
                                               std::numeric_limits<double>::quiet_NaN(),
                                               bar.date});
                    }
                }
                else if (std::abs(z) < _zExit && pos != 0)
                {
                    // revert to flat
                    if (pos > 0)
                    {
                        orders.push_back(Order{symbol, Side::Sell, pos,
                                               std::numeric_limits<double>::quiet_NaN(),
                                               bar.date});
                    }
                    else if (pos < 0)
                    {
                        orders.push_back(Order{symbol, Side::Buy, -pos,
                                               std::numeric_limits<double>::quiet_NaN(),
                                               bar.date});
                    }
                }

                return orders;
            }

        private:
            int _lookback;
            double _zEntry;
            double _zExit;
            int _unitSize;

            std::unordered_map<std::string, SymbolState> _state;
        };

    } // namespace

    std::unique_ptr<Strategy> makeMeanReversionStrategy(
        int lookbackWindow,
        double zEntry,
        double zExit,
        int unitSize)
    {
        return std::make_unique<MeanReversionStrategyImpl>(
            lookbackWindow, zEntry, zExit, unitSize);
    }

} // namespace qs
