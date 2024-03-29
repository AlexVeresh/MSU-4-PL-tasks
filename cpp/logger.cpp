#include "logger.hpp"
#include <iostream>

namespace msu_tasks_cpp

{
    using std::string;
    using std::to_string;

    const string Logger::path_to_string(const Path &path)
    {

        string result = "";

        for (int i = 0; i < path.total_length(); i++)
        {

            const auto cruise = graph_.get_cruise(path[i]);
            result += cruise.to_string(graph_);

            if (i != path.total_length() - 1)
            {
                result += '\n';
            }
        }

        return result;
    }

    const std::string Logger::paths_to_cities_to_string(const std::unordered_map<Graph::CityId, Path> &paths_to_cities)
    {

        string result = "";

        for (const auto &[city_to_id, path] : paths_to_cities)
        {
            const auto city_to = graph_.get_cities().at(city_to_id);
            result += "До города " + city_to.title + "\nПуть: " + path_to_string(path) + "\n\n";
        }

        return result;
    }

}
