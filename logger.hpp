#pragma once

#include "graph.hpp"
#include "path.hpp"

namespace msu_tasks_cpp
{

    struct Logger
    {
    public:
        explicit Logger(const Graph &graph) : graph_(graph){};

        const std::string paths_to_cities_to_string(const std::unordered_map<Graph::CityId, Path> &paths_to_cities);

        const std::string path_to_string(const Path &path);

    private:
        const Graph &graph_;
    };

}