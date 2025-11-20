#include "qs/DataFeed.hpp"
#include "qs/Logger.hpp"

#include <unordered_map>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <memory>

namespace qs
{

    namespace
    {

        Date parseDate(const std::string &s)
        {
            int y = 0, m = 0, d = 0;
            char dash1 = '-', dash2 = '-';
            std::stringstream ss(s);
            ss >> y >> dash1 >> m >> dash2 >> d;
            using namespace std::chrono;
            return sys_days{
                year{y} /
                month{static_cast<unsigned>(m)} /
                day{static_cast<unsigned>(d)}};
        };
        struct SymbolData
        {
            std::vector<Bar> bars;
        };

        class CsvDataFeedImpl : public DataFeed
        {
        public:
            CsvDataFeedImpl(std::vector<std::string> symbols,
                            std::string dataDirectory)
                : _symbols(std::move(symbols)),
                  _dataDirectory(std::move(dataDirectory))
            {
                loadAll();
            }

            bool next() override
            {
                if (_symbols.empty())
                    return false;
                if (_index + 1 >= _length)
                    return false;

                ++_index;
                _currentBars.clear();

                for (const auto &sym : _symbols)
                {
                    auto &data = _data.at(sym).bars;
                    if (_index < data.size())
                    {
                        const auto &bar = data[_index];
                        _currentBars.emplace_back(sym, bar);
                        _latestPrices[sym] = bar.close;
                    }
                }

                return !_currentBars.empty();
            }

            std::vector<std::pair<std::string, Bar>> currentBars() const override
            {
                return _currentBars;
            }

            double latestPrice(const std::string &symbol) const override
            {
                auto it = _latestPrices.find(symbol);
                if (it == _latestPrices.end())
                {
                    return std::numeric_limits<double>::quiet_NaN();
                }
                return it->second;
            }

        private:
            void loadAll()
            {
                _length = std::numeric_limits<std::size_t>::max();

                for (const auto &sym : _symbols)
                {
                    std::string path = _dataDirectory + "/" + sym + ".csv";
                    SymbolData sd;

                    std::ifstream in(path);
                    if (!in)
                    {
                        Logger::warn("CsvDataFeed: failed to open " + path);
                        continue;
                    }

                    std::string line;
                    // skip header
                    std::getline(in, line);

                    while (std::getline(in, line))
                    {
                        if (line.empty())
                            continue;
                        std::stringstream ss(line);
                        std::string token;

                        Bar bar;
                        std::getline(ss, token, ',');
                        bar.date = parseDate(token);
                        std::getline(ss, token, ',');
                        bar.open = std::stod(token);
                        std::getline(ss, token, ',');
                        bar.high = std::stod(token);
                        std::getline(ss, token, ',');
                        bar.low = std::stod(token);
                        std::getline(ss, token, ',');
                        bar.close = std::stod(token);
                        std::getline(ss, token, ',');
                        bar.volume = std::stod(token);

                        sd.bars.push_back(bar);
                    }

                    if (!sd.bars.empty())
                    {
                        _data.emplace(sym, std::move(sd));
                        if (sd.bars.size() < _length)
                        {
                            _length = sd.bars.size();
                        }
                        // initialize latest price with first bar
                        _latestPrices[sym] = sd.bars.front().close;
                    }
                    else
                    {
                        Logger::warn("CsvDataFeed: no data in " + path);
                    }
                }

                if (_length == std::numeric_limits<std::size_t>::max())
                {
                    _length = 0;
                }

                _index = 0;
                _currentBars.clear();
                if (_length > 0)
                {
                    for (const auto &sym : _symbols)
                    {
                        auto it = _data.find(sym);
                        if (it != _data.end() && !it->second.bars.empty())
                        {
                            const auto &bar = it->second.bars[_index];
                            _currentBars.emplace_back(sym, bar);
                            _latestPrices[sym] = bar.close;
                        }
                    }
                }
            }

            std::vector<std::string> _symbols;
            std::string _dataDirectory;
            std::unordered_map<std::string, SymbolData> _data;

            std::size_t _index{0};
            std::size_t _length{0};

            std::vector<std::pair<std::string, Bar>> _currentBars;
            std::unordered_map<std::string, double> _latestPrices;
        };

    } // namespace

    std::unique_ptr<DataFeed> makeCsvDataFeed(
        const std::vector<std::string> &symbols,
        const std::string &dataDirectory)
    {
        return std::make_unique<CsvDataFeedImpl>(symbols, dataDirectory);
    }

} // namespace qs
