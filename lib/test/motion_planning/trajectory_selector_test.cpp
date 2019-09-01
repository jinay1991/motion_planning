///
/// @file
///

#include <gtest/gtest.h>
#include <motion_planning/trajectory_builder.h>
#include <motion_planning/trajectory_prioritizer.h>
#include <motion_planning/trajectory_selector.h>

using namespace motion_planning;

namespace
{
TEST(TrajectorySelectorTest, GivenTypicalTrajectories_WhenPrioritized_ThenSelectedTopPriorityTrajectory)
{
    const auto ego_trajectory = TrajectoryBuilder().WithLaneId(Maneuver::LaneId::kEgo).WithCost(1.0).Build();
    const auto left_trajectory = TrajectoryBuilder().WithLaneId(Maneuver::LaneId::kLeft).WithCost(2.0).Build();
    const auto right_trajectory = TrajectoryBuilder().WithLaneId(Maneuver::LaneId::kRight).WithCost(3.0).Build();

    const auto trajectories = PlannedTrajectories{ego_trajectory, left_trajectory, right_trajectory};
    const auto prioritized_trajectories = TrajectoryPrioritizer().GetPrioritizedTrajectories(trajectories);
    const auto actual = TrajectorySelector().GetSelectedTrajectory(prioritized_trajectories);

    EXPECT_EQ(actual.maneuver.GetLaneId(), Maneuver::LaneId::kEgo);
    EXPECT_DOUBLE_EQ(actual.cost, ego_trajectory.cost);
}

}  // namespace
