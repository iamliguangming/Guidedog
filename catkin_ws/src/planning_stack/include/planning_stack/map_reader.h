#pragma once

#include <ros/ros.h>
#include <nav_msgs/MapMetaData.h>
#include <nav_msgs/OccupancyGrid.h>
#include "std_msgs/Header.h"
#include "nav_msgs/MapMetaData.h"
#include "nav_msgs/GetMap.h"
#include <vector>
#include <string>


class MapReader{
    private:
    ros::ServiceClient mapClient;
    std::string srv_name = "static_map";
    nav_msgs::GetMap mapSrv;
    nav_msgs::OccupancyGrid map;
    int height;
    int width;
    double resolution;
    int resolution_factor = 100;
    int resolution_int;
    std::vector<std::vector<bool>> occupancyGrid;
    std::vector<std::vector<bool>> local_occupancyGrid;

    public:
    MapReader(ros::NodeHandle* nh);
    ~MapReader();
    void request_map(ros::NodeHandle* nh);
    void get_map_data();
    int getHeight();
    int getWidth();
    double getResolution();
    bool get_occ_val(const double &x, const double &y);
    std::vector<std::vector<bool>> get_local_occ_grid(const double &x, const double &y, const double &r);
};