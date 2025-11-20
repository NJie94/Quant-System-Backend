#pragma once

#include <string>
#include <vector>

namespace qs
{

    struct Config
    {
        std::vector<std::string> symbols;
        std::string dataDirectory;
        double startingCash{100000.0};
        int shortWindow{20};
        int longWindow{50};

        static Config loadFromFile(const std::string &path);
    };

} // namespace qs
