#pragma once

#include <vector>
#include <unordered_map>
#include <string>

namespace msu_tasks_cpp

{

    class Graph
    {
    public:
        using CityId = int;
        using CruiseId = int;
        using TransportId = int;

        struct City
        {
        public:
            const CityId id;
            const std::string title;

            City(const CityId &_id, const std::string &_title) : id(_id), title(_title) {}

            void add_cruise_id(const CruiseId &id) { cruise_ids_.push_back(id); }

            const std::vector<CruiseId> &get_cruises_ids() const { return cruise_ids_; }

        private:
            std::vector<CruiseId> cruise_ids_;
        };

        struct Transport
        {
        public:
            const TransportId id;
            const std::string title;

            Transport(const TransportId &_id, const std::string &_title) : id(_id), title(_title) {}
        };

        struct Cruise
        {

            const CruiseId id;
            const CityId from_city_id;
            const CityId to_city_id;
            const TransportId transport_type_id;
            const unsigned int time;
            const unsigned int fare;

            Cruise(const CruiseId &_id,
                   const CityId &_from_city_id,
                   const CityId &_to_city_id,
                   const TransportId &_transport_type_id,
                   const unsigned int &_time,
                   const unsigned int &_fare) : id(_id), from_city_id(_from_city_id), to_city_id(_to_city_id), transport_type_id(_transport_type_id), time(_time), fare(_fare) {}
        };

        CityId add_city(const std::string &title);

        void add_cruise(const CityId &from_city_id, const CityId &to_city_id, const std::string &transport_title, const unsigned int &time, const unsigned int &fare);

        const std::vector<CruiseId> &get_city_cruises(const CityId &city_id) const { return cities_.at(city_id).get_cruises_ids(); }

        const Cruise &get_cruise(const CruiseId &id) const { return cruises_.at(id); }

        const Transport &get_transport(const TransportId &id) const { return transports_.at(id); }

        City &get_city(const CityId &id) { return cities_.at(id); }

        const std::unordered_map<CityId, City> &get_cities() const { return cities_; }

        const std::unordered_map<CruiseId, Cruise> &get_cruises() const { return cruises_; }

        const std::unordered_map<TransportId, Transport> &get_transports() const { return transports_; }

        const int get_cities_amount() const { return cities_.size(); }

        const int get_cruises_amount() const { return cruises_.size(); }

    private:
        std::unordered_map<CityId, City> cities_;
        std::unordered_map<CruiseId, Cruise> cruises_;
        std::unordered_map<TransportId, Transport> transports_;
        CityId city_id_counter_ = 0;
        CruiseId cruise_id_counter_ = 0;
        TransportId transport_id_counter_ = 0;

        CityId get_new_city_id() { return city_id_counter_++; }

        CruiseId get_new_cruise_id() { return cruise_id_counter_++; }

        TransportId get_new_transport_id() { return transport_id_counter_++; }
    };

}
