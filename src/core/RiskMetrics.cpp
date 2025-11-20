#include "qs/RiskMetrics.hpp"

#include <cmath>
#include <numeric>
#include <algorithm>

namespace qs
{

    RiskSummary RiskMetrics::summarize(const std::vector<double> &equityCurve,
                                       double periodsPerYear,
                                       double riskFreeRate)
    {
        RiskSummary s{};
        if (equityCurve.empty())
            return s;

        s.finalEquity = equityCurve.back();
        s.cagr = cagr(equityCurve, periodsPerYear);
        s.sharpe = sharpe(equityCurve, periodsPerYear, riskFreeRate);
        s.maxDrawdown = maxDrawdown(equityCurve);
        return s;
    }

    double RiskMetrics::cagr(const std::vector<double> &equityCurve,
                             double periodsPerYear)
    {
        if (equityCurve.size() < 2)
            return 0.0;

        double start = equityCurve.front();
        double end = equityCurve.back();
        if (start <= 0.0 || end <= 0.0)
            return 0.0;

        double nPeriods = static_cast<double>(equityCurve.size() - 1);
        double years = nPeriods / periodsPerYear;
        if (years <= 0.0)
            return 0.0;

        return std::pow(end / start, 1.0 / years) - 1.0;
    }

    double RiskMetrics::sharpe(const std::vector<double> &equityCurve,
                               double periodsPerYear,
                               double riskFreeRate)
    {
        if (equityCurve.size() < 2)
            return 0.0;

        std::vector<double> returns;
        returns.reserve(equityCurve.size() - 1);
        for (std::size_t i = 1; i < equityCurve.size(); ++i)
        {
            double r = (equityCurve[i] / equityCurve[i - 1]) - 1.0;
            returns.push_back(r);
        }

        double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / static_cast<double>(returns.size());

        double var = 0.0;
        for (double r : returns)
        {
            double diff = r - mean;
            var += diff * diff;
        }
        var /= static_cast<double>(returns.size());
        double stddev = std::sqrt(var);
        if (stddev <= 0.0)
            return 0.0;

        double rfPerPeriod = riskFreeRate / periodsPerYear;
        double excess = mean - rfPerPeriod;

        return excess / stddev * std::sqrt(periodsPerYear);
    }

    double RiskMetrics::maxDrawdown(const std::vector<double> &equityCurve)
    {
        if (equityCurve.empty())
            return 0.0;

        double peak = equityCurve.front();
        double maxDD = 0.0;
        for (double eq : equityCurve)
        {
            if (eq > peak)
                peak = eq;
            double dd = (eq - peak) / peak; // negative or zero
            if (dd < maxDD)
                maxDD = dd;
        }
        return maxDD; // expressed as negative fraction (e.g. -0.25 = -25%)
    }

} // namespace qs
