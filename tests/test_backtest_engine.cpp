#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "qs/BacktestEngine.hpp"
#include "qs/Strategy.hpp"
#include "qs/ExecutionModel.hpp"
#include "qs/DataFeed.hpp"
#include "qs/RiskMetrics.hpp"

using namespace qs;
using namespace std::chrono;

namespace
{

    // Minimal deterministic in-memory DataFeed for testing
    class DummyFeed : public DataFeed
    {
    public:
        DummyFeed()
        {
            // Single symbol, 10 days of simple prices
            for (int i = 0; i < 10; ++i)
            {
                Bar b;
                b.date = sys_days{year{2020} / month{static_cast<unsigned>(1)} / day{static_cast<unsigned>(2 + i)}};
                b.open = 100.0 + i;
                b.high = 100.0 + i + 1;
                b.low = 100.0 + i - 1;
                b.close = 100.0 + i;
                b.volume = 1000.0 + i;
                _bars.emplace_back("DEMO", b);
            }
            _latestPrice["DEMO"] = _bars.front().second.close;
            _index = 0;
        }

        bool next() override
        {
            if (_index + 1 >= _bars.size())
                return false;
            ++_index;
            _latestPrice["DEMO"] = _bars[_index].second.close;
            return true;
        }

        std::vector<std::pair<std::string, Bar>> currentBars() const override
        {
            return {_bars[_index]};
        }

        double latestPrice(const std::string &symbol) const override
        {
            auto it = _latestPrice.find(symbol);
            if (it == _latestPrice.end())
                return 0.0;
            return it->second;
        }

    private:
        std::vector<std::pair<std::string, Bar>> _bars;
        std::size_t _index{0};
        std::unordered_map<std::string, double> _latestPrice;
    };

    // Extremely simple "always long 100 shares" strategy to exercise engine
    class AlwaysLongStrategy : public Strategy
    {
    public:
        std::vector<Order> onBar(const std::string &symbol,
                                 const Bar &bar,
                                 const StrategyContext &ctx) override
        {
            std::vector<Order> orders;
            int pos = ctx.getPositionQty(symbol);
            if (pos == 0)
            {
                orders.push_back(Order{symbol, Side::Buy, 100,
                                       std::numeric_limits<double>::quiet_NaN(),
                                       bar.date});
            }
            return orders;
        }
    };

} // namespace

int main()
{
    auto data = std::make_unique<DummyFeed>();
    auto strat = std::make_unique<AlwaysLongStrategy>();
    auto exec = makeSimpleExecutionModel(1.0, 0.0);

    BacktestEngine engine(std::move(data), std::move(strat), std::move(exec), 100000.0);
    engine.run();

    const auto &eq = engine.equityCurve();
    assert(!eq.empty());
    assert(eq.back() > eq.front()); // prices increased

    auto summary = RiskMetrics::summarize(eq);
    std::cout << "Final equity: " << summary.finalEquity << "\n";
    std::cout << "CAGR: " << summary.cagr << "\n";
    std::cout << "Sharpe: " << summary.sharpe << "\n";
    std::cout << "Max DD: " << summary.maxDrawdown << "\n";

    std::cout << "test_backtest_engine: OK\n";
    return 0;
}
