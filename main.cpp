#include "qs/Config.hpp"
#include "qs/DataFeed.hpp"
#include "qs/Strategy.hpp"
#include "qs/ExecutionModel.hpp"
#include "qs/BacktestEngine.hpp"
#include "qs/RiskMetrics.hpp"
#include <iostream>

int main()
{
    using namespace qs;

    Config cfg = Config::loadFromFile(
        "examples/moving_average_crossover/ma_crossover_config.json");

    auto data = makeCsvDataFeed(cfg.symbols, cfg.dataDirectory);
    auto strat = makeMomentumStrategy(cfg.shortWindow, cfg.longWindow, /*unitSize=*/100);
    auto exec = makeSimpleExecutionModel(/*commissionPerTrade=*/1.0,
                                         /*slippagePerShare=*/0.01);

    BacktestEngine engine(std::move(data),
                          std::move(strat),
                          std::move(exec),
                          cfg.startingCash);
    engine.run();

    const auto &eq = engine.equityCurve();
    auto summary = RiskMetrics::summarize(eq);

    std::cout << "Final equity: " << summary.finalEquity << "\n";
    std::cout << "CAGR: " << summary.cagr << "\n";
    std::cout << "Sharpe: " << summary.sharpe << "\n";
    std::cout << "Max DD: " << summary.maxDrawdown << "\n";
}
