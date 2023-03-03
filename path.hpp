#pragma once

#include "graph.hpp"

namespace msu_tasks_cpp
{

    struct Path
    {
    public:
        Path(const Graph::CruiseId &_init_id)
        {
            cruise_ids_.push_back(_init_id);
        }

        Path() {}

        //const std::vector<Graph::CruiseId> &get_path_cruise_ids() const { return cruise_ids_; }

        const int total_length() const { return cruise_ids_.size(); }
        
        Path operator+(const Graph::Cruise &new_cruise);

        const Graph::CruiseId &operator[](int i) const { return cruise_ids_[i]; }

    private:
        std::vector<Graph::CruiseId> cruise_ids_;
    };

}
