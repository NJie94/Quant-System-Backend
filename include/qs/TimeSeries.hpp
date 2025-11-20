#pragma once

#include <vector>
#include <chrono>
#include <stdexcept>

#include "qs/Bar.hpp"

namespace qs
{

    class TimeSeries
    {
    public:
        void add(Date date, double value)
        {
            _dates.push_back(date);
            _values.push_back(value);
        }

        std::size_t size() const noexcept
        {
            return _values.size();
        }

        const std::vector<Date> &dates() const noexcept { return _dates; }
        const std::vector<double> &values() const noexcept { return _values; }

        double movingAverage(std::size_t window) const
        {
            if (window == 0 || window > _values.size())
            {
                throw std::invalid_argument("Invalid moving average window");
            }
            double sum = 0.0;
            for (std::size_t i = _values.size() - window; i < _values.size(); ++i)
            {
                sum += _values[i];
            }
            return sum / static_cast<double>(window);
        }

    private:
        std::vector<Date> _dates;
        std::vector<double> _values;
    };

} // namespace qs
