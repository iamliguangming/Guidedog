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
    std::vector<std::vector<bool>> occupancyGrid;

    public:
    MapReader(ros::NodeHandle* nh);
    ~MapReader();
    void request_map(ros::NodeHandle* nh);
    void get_map_data();
    int getHeight();
    int getWidth();
    double getResolution();
    bool get_occ_val(const int &i, const int &j);
};