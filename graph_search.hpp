#pragma once

#include <unordered_set>
#include "graph.hpp"
#include "path.hpp"

namespace msu_tasks_cpp
{

    class GraphSearch
    {
    public:
        GraphSearch(
            const Graph &graph,
            const std::unordered_set<Graph::TransportId> &restricted_transports) : graph_(graph),
                                                                                   restricted_transports_(restricted_transports){};

        const Path find_min_by_fare_among_shortest_path(const Graph::CityId &start_city_id,
                                                        const Graph::CityId &finish_city_id) const; // 1

        const Path find_min_by_fare_path(const Graph::CityId &start_city_id,
                                         const Graph::CityId &finish_city_id) const; // 2

        const Path find_min_by_cities_path(const Graph::CityId &start_city_id,
                                           const Graph::CityId &finish_city_id) const; // 3

        const std::unordered_map<Graph::CityId, Path> find_cities_in_limit_cost(const Graph::CityId &start_city_id,
                                                                                const unsigned long limit_cost) const; // 4

        const std::unordered_map<Graph::CityId, Path> find_cities_in_limit_time(const Graph::CityId &start_city_id,
                                                                                const unsigned long limit_time) const; // 5

    private:
        const Graph &graph_;
        const std::unordered_set<Graph::TransportId> &restricted_transports_;

        const bool is_transport_valid(const Graph::TransportId transportId) const
        {
            return !restricted_transports_.count(transportId);
        }
    };

}
