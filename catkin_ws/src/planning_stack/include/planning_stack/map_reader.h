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
    long int size;
    double resolution;
    int resolution_factor = 100;
    int resolution_int;
    std::vector<std::vector<bool>> occupancyGrid;
    std::vector<std::vector<bool>> local_occupancyGrid;
    int curr_grid_pos[2];
    int curr_pos_idx;

    public:
    MapReader(ros::NodeHandle* nh);
    ~MapReader();
    void request_map(ros::NodeHandle* nh);
    void get_map_data();
    int getHeight();
    int getWidth();
    long int get_map_size();
    double getResolution();
    bool get_occ_val(const double &x, const double &y);
    std::vector<std::vector<bool>> get_local_occ_grid(const double &x, const double &y, const double &r);
    int* get_curr_grid_pos(const double &x, const double &y);
    int get_curr_pos_idx(const double &x, const double &y);
    std::vector<int> get_coord_from_idx(const int &cell_idx);
    int get_idx_from_coord(const std::vector<int> &cell_coord);
    bool is_colliding(const std::vector<int> &cell_coord);
    bool is_outside_map(const std::vector<int> &cell_coord);

};