#pragma once

#include <vector>
#include <string>
#include <utility>

#include "qs/Order.hpp"
#include "qs/Trade.hpp"
#include "qs/Bar.hpp"

namespace qs
{

    class ExecutionModel
    {
    public:
        virtual ~ExecutionModel() = default;

        virtual std::vector<Trade> execute(
            const std::vector<Order> &orders,
            const std::vector<std::pair<std::string, Bar>> &bars) = 0;
    };

    // Factory helper for a simple execution model.
    std::unique_ptr<ExecutionModel> makeSimpleExecutionModel(
        double commissionPerTrade = 1.0,
        double slippagePerShare = 0.01);

} // namespace qs
