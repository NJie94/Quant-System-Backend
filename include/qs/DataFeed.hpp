#pragma once

#include <vector>
#include <string>
#include <utility>
#include "qs/Bar.hpp"

namespace qs
{

    class DataFeed
    {
    public:
        virtual ~DataFeed() = default;

        // Move to the next bar date. Returns false when finished.
        virtual bool next() = 0;

        // Bars for all symbols at current step.
        virtual std::vector<std::pair<std::string, Bar>> currentBars() const = 0;

        // Latest known price for a symbol.
        virtual double latestPrice(const std::string &symbol) const = 0;
    };

    // Factory for CSV-based data feed.
    // Implemented in src/data/CsvDataFeed.cpp
    //
    // Assumes per-symbol CSV files:
    //   <dataDirectory>/<symbol>.csv
    // Format:
    //   date,open,high,low,close,volume
    //   2020-01-02,100,101,99,100.5,123456
    std::unique_ptr<DataFeed> makeCsvDataFeed(
        const std::vector<std::string> &symbols,
        const std::string &dataDirectory);

} // namespace qs
