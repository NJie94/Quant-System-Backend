#pragma once

#include <vector>

namespace qs
{

    struct RiskSummary
    {
        double finalEquity{0.0};
        double cagr{0.0};
        double sharpe{0.0};
        double maxDrawdown{0.0};
    };

    class RiskMetrics
    {
    public:
        // equityCurve: sequence of portfolio equity values over time
        // periodsPerYear: e.g. 252 for daily data
        static RiskSummary summarize(const std::vector<double> &equityCurve,
                                     double periodsPerYear = 252.0,
                                     double riskFreeRate = 0.0);

        static double cagr(const std::vector<double> &equityCurve,
                           double periodsPerYear);

        static double sharpe(const std::vector<double> &equityCurve,
                             double periodsPerYear,
                             double riskFreeRate);

        static double maxDrawdown(const std::vector<double> &equityCurve);
    };

} // namespace qs
