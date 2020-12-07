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
    std::string map_frame;
    std::string world_frame = "world";
    int resolution_factor = 100;
    int resolution_int;
    std::vector<std::vector<bool>> occupancyGrid;
    std::vector<std::vector<bool>> local_occupancyGrid;
    std::vector<int> curr_grid_pos = {0, 0};
    int curr_pos_idx;
    int inflation_layer = 16;
    std::vector<std::vector<bool>> not_inflated_occupancyGrid;

    public:
    MapReader(ros::NodeHandle* nh);
    ~MapReader();
    void publish_transform();
    void request_map(ros::NodeHandle* nh);
    void get_map_data();
    void inflate_map(std::vector<std::vector<bool>> &occupancyGrid);
    int getHeight();
    int getWidth();
    long int get_map_size();
    double getResolution();
    std::string get_world_frame_id();
    bool get_occ_val(const std::vector<double> &xy_double);
    std::vector<std::vector<bool>> get_local_occ_grid(const std::vector<double> &xy_double, const double &r);
    std::vector<int> get_curr_grid_pos(const std::vector<double> &xy_double);
    int get_curr_pos_idx(const std::vector<double> &xy_double);
    std::vector<int> get_coord_from_idx(const int &cell_idx);
    int get_idx_from_coord(const std::vector<int> &cell_coord);
    std::vector<double> get_xy_double_from_idx(const int &curr_pos_idx);
    std::vector<double> get_xy_double_from_coord(const std::vector<int> &curr_grid_coord);
    bool is_colliding(const std::vector<int> &cell_coord, const bool &is_map_inflated);
    bool is_outside_map(const std::vector<int> &cell_coord);

};