/*
 * ElevationChangeDetection.hpp
 *
 *  Created on: Mar 26, 2015
 *      Author: Martin Wermelinger
 *	 Institute: ETH Zurich, Autonomous Systems Lab
 */

#pragma once

// Grid Map
#include <grid_map/grid_map.hpp>

// ROS
#include <ros/ros.h>
#include <tf/transform_listener.h>

#include <elevation_change_msgs/DetectObstacle.h>
#include <elevation_change_msgs/ObstacleResult.h>
#include <traversability_msgs/FootprintPath.h>

// STL
#include <vector>
#include <string>
#include <algorithm>

namespace elevation_change_detection {

  class ElevationChangeDetection {

  public:
    /*!
    * Constructor.
    */
    ElevationChangeDetection(ros::NodeHandle& nodeHandle);

    /*!
    * Destructor.
    */
    virtual ~ElevationChangeDetection();

    /*!
     * Publishes the (latest) elevation change map.
     * @return true if successful.
     */
    bool publishElevationChangeMap(const grid_map::GridMap& map);

    /*!
     * Publishes the ground truth map.
     * @return true if successful.
     */
    bool publishGroundTruthMap(const grid_map::GridMap& map);

  private:

    /*!
    * Reads and verifies the ROS parameters.
    * @return true if successful.
    */
    bool readParameters();

    /*!
    * Loads the ground truth elevation map from a ROS bag file.
    * @param[in] pathToBag string with the path to the Bag file containing the ground truth data
    * @return true if successful.
    */
    bool loadElevationMap(const std::string& pathToBag, const std::string& topicName);

    /*!
    * Callback function for the update timer. Forces an update of the elevation
    * change map from a new elevation map requested from the grid map service.
    * @param timerEvent the timer event.
    */
    void updateTimerCallback(const ros::TimerEvent& timerEvent);

    /*!
     * ROS service callback function to detect obstacles on a footprint path.
     * @param request the ROS service request defining footprint paths.
     * @param response the ROS service response containing arrays with obstacles on the paths.
     * @return true if successful.
     */
    bool detectObstacle(
      elevation_change_msgs::DetectObstacle::Request& request,
      elevation_change_msgs::DetectObstacle::Response& response);

    /*!
     * ROS service callback function to detect obstacles on a footprint path.
     * @param request the ROS service request defining footprint path.
     * @param response the ROS service response containing an array with obstacles on the path.
     * @return true if successful.
     */
    bool checkPathForObstacles(const traversability_msgs::FootprintPath& path, std::vector<elevation_change_msgs::Obstacle>& obstacles);

    /*!
     * Checks the polygon for obstacles.
     * @param[in] polygon the polygon to verify.
     * @param[in] map the current elevation change map.
     * @param[out] obstacles vector of found obstacles.
     * @return true if obstacle found within polygon.
     */
    bool checkPolygonForObstacles(const grid_map::Polygon& polygon, grid_map::GridMap& map, std::vector<elevation_change_msgs::Obstacle>& obstacles);

    /*!
     * Gets the grid map for the desired submap defined by position and length.
     * @param[in] position position of the submap.
     * @param[in] length length of the submap.
     * @param[out] map the map that is received.
     * @return true if successful, false if ROS service call failed.
     */
    bool getGridMap(const grid_map::Position& position, const grid_map::Length& length, grid_map_msgs::GridMap& map);

    /*!
     * Compute the elevation change map and add it to the elevation map.
     * @param elevationMap map where the elevation change map is added.
     */
    void computeElevationChange(grid_map::GridMap& elevationMap);

    //! ROS node handle.
    ros::NodeHandle& nodeHandle_;

    //! Elevation map service client.
    ros::ServiceClient submapClient_;

    //! Obstacle detection service server.
    ros::ServiceServer obstacleDetectionService_;

    //! Name of the elevation submap service.
    std::string submapServiceName_;

    //! Timer for the map update.
    ros::Timer updateTimer_;

    //! Duration between map updates.
    ros::Duration updateDuration_;

    //! Center point of the requested map.
    geometry_msgs::PointStamped submapPoint_;

    //! Id of the frame of the elevation map.
    std::string mapFrameId_;

    //! TF listener.
    tf::TransformListener transformListener_;

    //! Requested map cell types.
    std::vector<std::string> requestedMapTypes_;

    //! Requested map length in [m].
    grid_map::Length mapLength_;

    //! Elevation map type.
    const std::string layer_;

    //! Elevation change map type.
    const std::string elevationChangeLayer_;

    //! Publisher of elevation change map.
    ros::Publisher elevationChangePublisher_;

    //! Ground Truth elevation map
    grid_map::GridMap groundTruthMap_;

    //! Publisher of the ground truth map.
    ros::Publisher groundTruthPublisher_;

    //! Threshold for minimal elevation difference
    double threshold_;

  };

} /* namespace elevation_change_detection*/
