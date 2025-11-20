#pragma once

#include <memory>
#include <vector>
#include <string>

#include "Strategy.hpp"
#include "DataFeed.hpp"
#include "qs/Portfolio.hpp"
#include "qs/ExecutionModel.hpp"
#include "qs/Bar.hpp"

namespace qs
{

    class BacktestEngine : public StrategyContext
    {
    public:
        BacktestEngine(std::unique_ptr<DataFeed> data,
                       std::unique_ptr<Strategy> strategy,
                       std::unique_ptr<ExecutionModel> exec,
                       double startingCash);

        void run();

        // StrategyContext
        double getPrice(const std::string &symbol) const override;
        int getPositionQty(const std::string &symbol) const override;
        double getCash() const override;

        const std::vector<Date> &dates() const noexcept { return _dates; }
        const std::vector<double> &equityCurve() const noexcept { return _equityCurve; }

    private:
        std::unique_ptr<DataFeed> _data;
        std::unique_ptr<Strategy> _strategy;
        std::unique_ptr<ExecutionModel> _exec;
        Portfolio _portfolio;

        std::vector<Date> _dates;
        std::vector<double> _equityCurve;
    };

} // namespace qs
