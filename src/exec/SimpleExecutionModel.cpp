#include "qs/ExecutionModel.hpp"

#include <unordered_map>
#include <limits>
#include <memory>

namespace qs
{

    namespace
    {

        class SimpleExecutionModelImpl : public ExecutionModel
        {
        public:
            SimpleExecutionModelImpl(double commissionPerTrade,
                                     double slippagePerShare)
                : _commissionPerTrade(commissionPerTrade),
                  _slippagePerShare(slippagePerShare) {}

            std::vector<Trade> execute(
                const std::vector<Order> &orders,
                const std::vector<std::pair<std::string, Bar>> &bars) override
            {
                std::unordered_map<std::string, Bar> barMap;
                for (const auto &kv : bars)
                {
                    barMap.emplace(kv.first, kv.second);
                }

                std::vector<Trade> trades;
                trades.reserve(orders.size());

                for (const auto &o : orders)
                {
                    auto it = barMap.find(o.symbol);
                    if (it == barMap.end())
                    {
                        // no price for this symbol at this step; skip
                        continue;
                    }
                    const Bar &bar = it->second;

                    double basePrice = bar.close;
                    double signedDir = (o.side == Side::Buy ? 1.0 : -1.0);
                    double price = basePrice + signedDir * _slippagePerShare;
                    double commission = _commissionPerTrade;
                    double slippage = _slippagePerShare * static_cast<double>(o.quantity);

                    trades.push_back(Trade{
                        o.symbol,
                        o.side,
                        o.quantity,
                        price,
                        o.date,
                        commission,
                        slippage});
                }

                return trades;
            }

        private:
            double _commissionPerTrade;
            double _slippagePerShare;
        };

    } // namespace

    std::unique_ptr<ExecutionModel> makeSimpleExecutionModel(
        double commissionPerTrade,
        double slippagePerShare)
    {
        return std::make_unique<SimpleExecutionModelImpl>(
            commissionPerTrade, slippagePerShare);
    }

} // namespace qs
