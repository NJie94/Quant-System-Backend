#include "qs/Config.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace qs
{

    namespace
    {

        std::string readFile(const std::string &path)
        {
            std::ifstream in(path);
            if (!in)
            {
                throw std::runtime_error("Failed to open config file: " + path);
            }
            std::ostringstream ss;
            ss << in.rdbuf();
            return ss.str();
        }

        std::string trim(const std::string &s)
        {
            auto start = s.find_first_not_of(" \t\r\n");
            auto end = s.find_last_not_of(" \t\r\n");
            if (start == std::string::npos)
                return "";
            return s.substr(start, end - start + 1);
        }

        std::string extractString(const std::string &json, const std::string &key)
        {
            auto pos = json.find("\"" + key + "\"");
            if (pos == std::string::npos)
                return "";
            pos = json.find(':', pos);
            if (pos == std::string::npos)
                return "";
            pos = json.find('"', pos);
            if (pos == std::string::npos)
                return "";
            auto end = json.find('"', pos + 1);
            if (end == std::string::npos)
                return "";
            return json.substr(pos + 1, end - pos - 1);
        }

        double extractNumber(const std::string &json, const std::string &key, double defVal)
        {
            auto pos = json.find("\"" + key + "\"");
            if (pos == std::string::npos)
                return defVal;
            pos = json.find(':', pos);
            if (pos == std::string::npos)
                return defVal;
            auto end = json.find_first_of(",}", pos + 1);
            if (end == std::string::npos)
                end = json.size();
            std::string numStr = trim(json.substr(pos + 1, end - pos - 1));
            try
            {
                return std::stod(numStr);
            }
            catch (...)
            {
                return defVal;
            }
        }

        std::vector<std::string> extractStringArray(const std::string &json, const std::string &key)
        {
            std::vector<std::string> result;
            auto pos = json.find("\"" + key + "\"");
            if (pos == std::string::npos)
                return result;
            pos = json.find('[', pos);
            if (pos == std::string::npos)
                return result;
            auto end = json.find(']', pos);
            if (end == std::string::npos)
                return result;

            std::string body = json.substr(pos + 1, end - pos - 1);
            std::stringstream ss(body);
            std::string token;
            while (std::getline(ss, token, ','))
            {
                token = trim(token);
                if (!token.empty() && token.front() == '"' && token.back() == '"')
                {
                    result.push_back(token.substr(1, token.size() - 2));
                }
            }
            return result;
        }

    } // namespace

    Config Config::loadFromFile(const std::string &path)
    {
        Config cfg;
        std::string json = readFile(path);

        cfg.dataDirectory = extractString(json, "data_directory");
        if (cfg.dataDirectory.empty())
        {
            cfg.dataDirectory = "data";
        }

        cfg.symbols = extractStringArray(json, "symbols");
        if (cfg.symbols.empty())
        {
            cfg.symbols.push_back("DEMO");
        }

        cfg.startingCash = extractNumber(json, "starting_cash", 100000.0);
        cfg.shortWindow = static_cast<int>(extractNumber(json, "short_window", 20));
        cfg.longWindow = static_cast<int>(extractNumber(json, "long_window", 50));

        return cfg;
    }

} // namespace qs
