#pragma once

#include <string>
#include <vector>
#include <memory>
#include "qs/Bar.hpp"
#include "qs/Order.hpp"

namespace qs
{

    class StrategyContext
    {
    public:
        virtual ~StrategyContext() = default;

        virtual double getPrice(const std::string &symbol) const = 0;
        virtual int getPositionQty(const std::string &symbol) const = 0;
        virtual double getCash() const = 0;
    };

    class Strategy
    {
    public:
        virtual ~Strategy() = default;

        virtual std::vector<Order> onBar(const std::string &symbol,
                                         const Bar &bar,
                                         const StrategyContext &ctx) = 0;

        virtual void onStart() {}
        virtual void onFinish() {}
    };

    // Factory helpers for built-in strategies (implemented in .cpps)
    std::unique_ptr<Strategy> makeMeanReversionStrategy(
        int lookbackWindow,
        double zEntry,
        double zExit,
        int unitSize);

    std::unique_ptr<Strategy> makeMomentumStrategy(
        int shortWindow,
        int longWindow,
        int unitSize);

} // namespace qs
