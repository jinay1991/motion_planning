///
/// @file
///
#include <sstream>

#include "logging/logging.h"
#include "motion_planning/trajectory_evaluator.h"

namespace motion_planning
{
TrajectoryEvaluator::TrajectoryEvaluator(std::shared_ptr<IDataSource>& data_source)
    : lane_evaluator_{std::make_unique<LaneEvaluator>(data_source)}
{
}

Trajectories TrajectoryEvaluator::GetRatedTrajectories(const Trajectories& optimized_trajectories) const
{
    Trajectories rated_trajectories{};

    // discard invalid lane trajectories
    std::copy_if(optimized_trajectories.begin(), optimized_trajectories.end(), std::back_inserter(rated_trajectories),
                 [&](const auto& trajectory) { return lane_evaluator_->IsValidLane(trajectory.lane_id); });

    // update costs for each trajectory
    const auto adjust_costs = [&](const auto& trajectory) {
        auto rated_trajectory = trajectory;
        if (!lane_evaluator_->IsDrivableLane(trajectory.lane_id))
        {
            rated_trajectory.cost = std::numeric_limits<double>::infinity();
        }
        else if (trajectory.lane_id != LaneId::kEgo)
        {
            rated_trajectory.cost += 1;
        }
        else
        {
            // Ego Lane cost to minimal if drivable (i.e. cost=0)
        }

        return rated_trajectory;
    };
    std::transform(rated_trajectories.begin(), rated_trajectories.end(), rated_trajectories.begin(), adjust_costs);

    std::stringstream log_stream;
    log_stream << "Evaluated trajectories: " << rated_trajectories.size() << std::endl;
    std::for_each(rated_trajectories.begin(), rated_trajectories.end(),
                  [&log_stream](const auto& trajectory) { log_stream << " (+) " << trajectory << std::endl; });
    LOG_DEBUG("TrajectoryEvaluator", log_stream.str());
    return rated_trajectories;
}

}  // namespace motion_planning
