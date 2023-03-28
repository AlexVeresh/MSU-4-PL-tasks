#include <set>
#include <vector>
#include <iostream>
#include <limits>
#include <queue>
#include "graph.hpp"
#include "graph_search.hpp"
#include "logger.hpp"

namespace msu_tasks_cpp
{

    using std::make_pair;
    using std::pair;
    using std::queue;
    using std::set;
    using std::unordered_map;
    using std::vector;

    using std::cout;
    using std::endl;

    using CityId = Graph::CityId;
    const unsigned long MAX_VALUE = std::numeric_limits<long>::max();

    const Path GraphSearch::find_min_by_fare_among_shortest_path(const Graph::CityId &start_city_id,
                                                                 const Graph::CityId &finish_city_id) const
    {

        auto logger = Logger(graph_);

        const unsigned int cities_count = graph_.get_cities().size();

        vector<unsigned long> distancies_to_cities(cities_count, MAX_VALUE);
        vector<unsigned long> total_fares_to_cities(cities_count, MAX_VALUE);
        distancies_to_cities[start_city_id] = 0;
        total_fares_to_cities[start_city_id] = 0;

        vector<Path> pathes_to_all_cities(cities_count);

        set<pair<unsigned long, CityId>> current_city_ids;
        current_city_ids.insert(make_pair(distancies_to_cities[start_city_id], start_city_id));

        while (!current_city_ids.empty())
        {
            const auto from_city_id = current_city_ids.begin()->second;
            current_city_ids.erase(current_city_ids.begin());

            for (const auto &cruise_id : graph_.get_cities().at(from_city_id).get_cruises_ids())
            {
                const auto to_city_id = graph_.get_cruise(cruise_id).to_city_id;
                const auto time = graph_.get_cruise(cruise_id).time;
                const auto fare = graph_.get_cruise(cruise_id).fare;
                const auto transport_type_id = graph_.get_cruise(cruise_id).transport_type_id;

                if (is_transport_valid(transport_type_id))
                {
                    if (distancies_to_cities[from_city_id] + time < distancies_to_cities[to_city_id])
                    {
                        current_city_ids.erase(make_pair(distancies_to_cities[to_city_id], to_city_id));
                        distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + time;
                        total_fares_to_cities[to_city_id] = total_fares_to_cities[from_city_id] + fare;
                        pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + graph_.get_cruise(cruise_id);
                        current_city_ids.insert(make_pair(distancies_to_cities[to_city_id], to_city_id));
                    }
                    else if (distancies_to_cities[from_city_id] + time == distancies_to_cities[to_city_id])
                    {
                        const auto &current_fare = total_fares_to_cities[to_city_id];
                        const auto &new_fare = total_fares_to_cities[from_city_id] + fare;
                        if (new_fare < current_fare)
                        {
                            current_city_ids.erase(make_pair(distancies_to_cities[to_city_id], to_city_id));
                            distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + time;
                            total_fares_to_cities[to_city_id] = total_fares_to_cities[from_city_id] + fare;
                            pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + graph_.get_cruise(cruise_id);
                            current_city_ids.insert(make_pair(distancies_to_cities[to_city_id], to_city_id));
                        }
                    }
                }
            }
        }

        return pathes_to_all_cities[finish_city_id];
    }

    const Path GraphSearch::find_min_by_fare_path(const Graph::CityId &start_city_id,
                                                  const Graph::CityId &finish_city_id) const
    {

        auto logger = Logger(graph_);

        const unsigned int cities_count = graph_.get_cities().size();

        vector<unsigned long> distancies_to_cities(cities_count, MAX_VALUE);
        distancies_to_cities[start_city_id] = 0;

        vector<Path> pathes_to_all_cities(cities_count);

        set<pair<unsigned long, CityId>> current_city_ids;
        current_city_ids.insert(make_pair(distancies_to_cities[start_city_id], start_city_id));

        while (!current_city_ids.empty())
        {
            const auto from_city_id = current_city_ids.begin()->second;
            current_city_ids.erase(current_city_ids.begin());

            for (const auto &cruise_id : graph_.get_cities().at(from_city_id).get_cruises_ids())
            {
                const auto to_city_id = graph_.get_cruise(cruise_id).to_city_id;
                const auto fare = graph_.get_cruise(cruise_id).fare;
                const auto transport_type_id = graph_.get_cruise(cruise_id).transport_type_id;

                if (distancies_to_cities[from_city_id] + fare < distancies_to_cities[to_city_id] &&
                    is_transport_valid(transport_type_id))
                {
                    current_city_ids.erase(make_pair(distancies_to_cities[to_city_id], to_city_id));
                    distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + fare;
                    pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + graph_.get_cruise(cruise_id);
                    current_city_ids.insert(make_pair(distancies_to_cities[to_city_id], to_city_id));
                }
            }
        }

        return pathes_to_all_cities[finish_city_id];
    }

    const Path GraphSearch::find_min_by_cities_path(const Graph::CityId &start_city_id,
                                                    const Graph::CityId &finish_city_id) const
    {

        auto logger = Logger(graph_);

        const unsigned int cities_count = graph_.get_cities().size();

        vector<unsigned long> distancies_to_cities(cities_count, MAX_VALUE);
        distancies_to_cities[start_city_id] = 0;

        vector<Path> pathes_to_all_cities(cities_count);

        // set<pair<unsigned long, CityId>> current_city_ids;
        // current_city_ids.insert(make_pair(distancies_to_cities[start_city_id], start_city_id));

        queue<CityId> current_city_ids;
        current_city_ids.push(start_city_id);
        vector<bool> used(cities_count);
        used[start_city_id] = true;

        while (!current_city_ids.empty())
        {
            const auto from_city_id = current_city_ids.front();
            current_city_ids.pop();
            for (const auto &cruise_id : graph_.get_cities().at(from_city_id).get_cruises_ids())
            {
                const auto to_city_id = graph_.get_cruise(cruise_id).to_city_id;
                const auto transport_type_id = graph_.get_cruise(cruise_id).transport_type_id;

                if (!used[to_city_id] && is_transport_valid(transport_type_id))
                {
                    used[to_city_id] = true;
                    current_city_ids.push(to_city_id);
                    distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + 1;
                    pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + graph_.get_cruise(cruise_id);
                }
            }
        }

        // while (!current_city_ids.empty())
        // {
        //     const auto from_city_id = current_city_ids.begin()->second;
        //     current_city_ids.erase(current_city_ids.begin());

        //     for (const auto &cruise_id : graph_.get_cities().at(from_city_id).get_cruises_ids())
        //     {
        //         const auto to_city_id = graph_.get_cruise(cruise_id).to_city_id;
        //         const auto transport_type_id = graph_.get_cruise(cruise_id).transport_type_id;

        //         if (distancies_to_cities[from_city_id] + 1 < distancies_to_cities[to_city_id] &&
        //             is_transport_valid(transport_type_id))
        //         {
        //             current_city_ids.erase(make_pair(distancies_to_cities[to_city_id], to_city_id));
        //             distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + 1;
        //             pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + graph_.get_cruise(cruise_id);
        //             current_city_ids.insert(make_pair(distancies_to_cities[to_city_id], to_city_id));
        //         }
        //     }
        // }

        return pathes_to_all_cities[finish_city_id];
    }

    const unordered_map<Graph::CityId, Path> GraphSearch::find_cities_in_limit_cost(const Graph::CityId &start_city_id,
                                                                                    const unsigned long limit_cost) const
    {

        auto logger = Logger(graph_);

        const unsigned int cities_count = graph_.get_cities().size();

        vector<unsigned long> distancies_to_cities(cities_count, MAX_VALUE);
        distancies_to_cities[start_city_id] = 0;

        vector<Path> pathes_to_all_cities(cities_count);

        set<pair<unsigned long, CityId>> current_city_ids;
        current_city_ids.insert(make_pair(distancies_to_cities[start_city_id], start_city_id));

        while (!current_city_ids.empty())
        {
            const auto from_city_id = current_city_ids.begin()->second;
            current_city_ids.erase(current_city_ids.begin());

            for (const auto &cruise_id : graph_.get_cities().at(from_city_id).get_cruises_ids())
            {
                const auto to_city_id = graph_.get_cruise(cruise_id).to_city_id;
                const auto fare = graph_.get_cruise(cruise_id).fare;
                const auto transport_type_id = graph_.get_cruise(cruise_id).transport_type_id;

                if (distancies_to_cities[from_city_id] + fare < distancies_to_cities[to_city_id] &&
                    is_transport_valid(transport_type_id))
                {
                    current_city_ids.erase(make_pair(distancies_to_cities[to_city_id], to_city_id));
                    distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + fare;
                    pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + graph_.get_cruise(cruise_id);
                    current_city_ids.insert(make_pair(distancies_to_cities[to_city_id], to_city_id));
                }
            }
        }

        unordered_map<Graph::CityId, Path> result;

        for (int i = 0; i < distancies_to_cities.size(); i++)
        {
            const auto total_fare = distancies_to_cities[i];

            if (total_fare <= limit_cost && total_fare != 0)
            {
                result.emplace(i, pathes_to_all_cities[i]);
            }
        }

        return result;
    }

    const unordered_map<Graph::CityId, Path> GraphSearch::find_cities_in_limit_time(const Graph::CityId &start_city_id,
                                                                                    const unsigned long limit_time) const
    {

        auto logger = Logger(graph_);

        const unsigned int cities_count = graph_.get_cities().size();

        vector<unsigned long> distancies_to_cities(cities_count, MAX_VALUE);
        distancies_to_cities[start_city_id] = 0;

        vector<Path> pathes_to_all_cities(cities_count);

        set<pair<unsigned long, CityId>> current_city_ids;
        current_city_ids.insert(make_pair(distancies_to_cities[start_city_id], start_city_id));

        while (!current_city_ids.empty())
        {
            const auto from_city_id = current_city_ids.begin()->second;
            current_city_ids.erase(current_city_ids.begin());

            for (const auto &cruise_id : graph_.get_cities().at(from_city_id).get_cruises_ids())
            {
                const auto to_city_id = graph_.get_cruise(cruise_id).to_city_id;
                const auto time = graph_.get_cruise(cruise_id).time;
                const auto transport_type_id = graph_.get_cruise(cruise_id).transport_type_id;

                if (distancies_to_cities[from_city_id] + time < distancies_to_cities[to_city_id] &&
                    is_transport_valid(transport_type_id))
                {
                    current_city_ids.erase(make_pair(distancies_to_cities[to_city_id], to_city_id));
                    distancies_to_cities[to_city_id] = distancies_to_cities[from_city_id] + time;
                    pathes_to_all_cities[to_city_id] = pathes_to_all_cities[from_city_id] + graph_.get_cruise(cruise_id);
                    current_city_ids.insert(make_pair(distancies_to_cities[to_city_id], to_city_id));
                }
            }
        }

        unordered_map<Graph::CityId, Path> result;

        for (int i = 0; i < distancies_to_cities.size(); i++)
        {
            const auto total_time = distancies_to_cities[i];

            if (total_time <= limit_time && total_time != 0)
            {
                result.emplace(i, pathes_to_all_cities[i]);
            }
        }

        return result;
    }
}