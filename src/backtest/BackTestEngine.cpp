#include "qs/BacktestEngine.hpp"

namespace qs
{
    BacktestEngine::BacktestEngine(std::unique_ptr<DataFeed> data,
                                   std::unique_ptr<Strategy> strategy,
                                   std::unique_ptr<ExecutionModel> exec,
                                   double startingCash)
        : _data(std::move(data)),
          _strategy(std::move(strategy)),
          _exec(std::move(exec)),
          _portfolio(startingCash) {}

    double BacktestEngine::getPrice(const std::string &symbol) const
    {
        return _data->latestPrice(symbol);
    }

    // TODO: wire portfolio to expose positions
    int BacktestEngine::getPositionQty(const std::string &symbol) const
    {
        return 0;
    }

    double BacktestEngine::getCash() const
    {
        return _portfolio.cash();
    }

    void BacktestEngine::run()
    {
        _strategy->onStart();

        while (_data->next())
        {
            auto bars = _data->currentBars();

            std::vector<Order> allOrders;
            for (auto &[symbol, bar] : bars)
            {
                auto orders = _strategy->onBar(symbol, bar, *this);
                allOrders.insert(allOrders.end(), orders.begin(), orders.end());
            }

            auto trades = _exec->execute(allOrders, bars);
            for (auto &t : trades)
            {
                _portfolio.applyTrade(t);
            }

            _portfolio.markToMarket([this](const std::string &sym)
                                    { return getPrice(sym); });

            // add on storage for equity and log
        }

        _strategy->onFinish();
    }

}
