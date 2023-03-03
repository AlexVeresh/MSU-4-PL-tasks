#include "graph.hpp"
#include <unordered_map>
#include <vector>
#include <iostream>

namespace msu_tasks_cpp

{

    using std::string;
    using std::vector;

    Graph::CityId Graph::add_city(const string &title)
    {
        CityId city_id = -1;
        for (const auto &[key, value] : cities_)
        {
            if (value.title == title)
            {
                city_id = key;
                break;
            }
        }

        if (city_id == -1)
        {
            city_id = get_new_city_id();
            cities_.emplace(city_id, City(city_id, title));
        }
        return city_id;
    }

    void Graph::add_cruise(const CityId &from_city_id, const CityId &to_city_id, const string &transport_title, const unsigned int &time, const unsigned int &fare)
    {
        const CruiseId new_cruise_id = get_new_cruise_id();
        TransportId transport_id = -1;
        for (const auto &[key, value] : transports_)
        {
            if (value.title == transport_title)
            {
                transport_id = key;
                break;
            }
        }

        if (transport_id == -1)
        {
            transport_id = get_new_transport_id();
            transports_.emplace(transport_id, Transport(transport_id, transport_title));
        }
        cruises_.emplace(new_cruise_id, Cruise(new_cruise_id, from_city_id, to_city_id, transport_id, time, fare));
        get_city(from_city_id).add_cruise_id(new_cruise_id);
    }
}
