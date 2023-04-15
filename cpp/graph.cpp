#include "graph.hpp"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iostream>

namespace msu_tasks_cpp

{

    using std::string;
    using std::vector;

    Graph::CityId Graph::add_city(const string &title)
    {
        const auto it = titles_to_ids_.find(title);
        if (it != titles_to_ids_.end())
        {
            return it->second;
        }
        else
        {
            const auto city_id = get_new_city_id();
            cities_.emplace(city_id, City(city_id, title));
            titles_to_ids_.emplace(title, city_id);
            return city_id;
        }
    }

    Graph::TransportId Graph::add_transport(const std::string &title)
    {
        TransportId transport_id = -1;
        for (const auto &[key, value] : transports_)
        {
            if (value.title == title)
            {
                transport_id = key;
                break;
            }
        }

        if (transport_id == -1)
        {
            transport_id = get_new_transport_id();
            transports_.emplace(transport_id, Transport(transport_id, title));
        }
        return transport_id;
    }

    void Graph::add_cruise(Cruise &cruise)
    {
        const CruiseId new_cruise_id = get_new_cruise_id();
        cruise.id = new_cruise_id;
        cruises_.emplace(new_cruise_id, cruise);
        get_city(cruise.from_city_id).add_cruise_id(new_cruise_id);
    }

    const std::string Graph::Cruise::to_string(const Graph &graph) const
    {

        const auto city_from = graph.get_cities().at(from_city_id);
        const auto city_to = graph.get_cities().at(to_city_id);

        return city_from.title + " ------> " + city_to.title + " (Время: " + std::to_string(time) + " у. е. | Цена: " + std::to_string(fare) + " ₽)";
    };
}
