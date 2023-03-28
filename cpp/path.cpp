#include "path.hpp"

namespace msu_tasks_cpp

{

    Path Path::operator+(const Graph::Cruise &new_cruise)
    {
        Path result;
        result.cruise_ids_ = cruise_ids_;
        result.cruise_ids_.push_back(new_cruise.id);
        return result;
    }

}
