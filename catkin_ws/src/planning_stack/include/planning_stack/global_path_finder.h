#pragma once

#include <string>
#include <list>
#include <set>
#include <utility>
#include "map_reader.h"
#include <nav_msgs/Path.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Pose2D.h>
#include <nav_msgs/Odometry.h>

 

class GlobalPathFinder{
    private:
    std::string method;
    std::list<std::pair<int, int>> parents;  // first int stores parent index, second stores cell index.
    std::set<std::pair<int, int>> cost_min_heap;  // Stores the unvisited cells. first int stores weight, second stores cell index.
    std::list<std::pair<int, int>> curr_adjacent_list;  // first int stores distance, second stores cell index.
    std::vector<int> cost;  // cost or weight of each cell w.r.t start point
    MapReader* map;
    nav_msgs::Path path;
    ros::Publisher path_pub;
    ros::Subscriber nav_goal_sub;
    ros::Subscriber odom_sub;
    std::string path_topic = "/global_path";
    std::string nav_goal_topic = "/move_base_simple/goal";
    ros::NodeHandle *nh;
    std::string world_frame;
    std::vector<double> goal_received;
    std::vector<double> goal_received_pre = {10000.0, 10000.0};
    std::vector<double> initial_pose;
    geometry_msgs::Pose2D bot_pos;
    bool is_goal_received = false;
    bool print_once_flag = true;
    double map_offset_x = 115.0;
    double map_offset_y = 115.0;

    public:
    GlobalPathFinder(ros::NodeHandle *nh, const std::string &method, MapReader* input_map);
    void receive_goal(const geometry_msgs::PoseStamped &nav_goal_pose);
    void add_adjacent_cell(const int &cell_idx);
    // std::vector<std::vector<int>> find_path(const std::vector<int> start_cell_coord, const std::vector<int> goal_cell_coord);
    void find_path(const std::vector<double> &start_cell, const std::vector<double> &goal_cell);
    void publish_path();
    nav_msgs::Path fireup();
    nav_msgs::Path get_path();
    void Odom_call_back(const nav_msgs::Odometry::ConstPtr &msg);
};