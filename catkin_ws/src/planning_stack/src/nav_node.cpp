#include "map_reader.h"
#include "global_path_finder.h"
#include <ros/ros.h>
#include <iostream>
#include <vector>


int main(int argc, char** argv){
    ros::init(argc, argv, "nav_node");
    ros::NodeHandle nh;
    MapReader map(&nh);

    //============================Map Test Section===============================

    // Test get_occ_val for global occupancy grid 
    ROS_INFO("Testing get_occ_val...");
    for(int i=500; i<=600; i++){
        ROS_INFO("Current occ value is: %d", map.get_occ_val(i*0.3, 250*0.3));
    }

    // Test local occupancy grid
    ROS_INFO("Testing get_local_occ_grid...");
    std::vector<std::vector<bool>> local_grid = map.get_local_occ_grid(150, 75, 0.5);
    ROS_INFO("local map grid value:");
    std::vector<std::vector<bool>>::iterator row;
    std::vector<bool>::iterator col;
    for(row = local_grid.begin(); row != local_grid.end(); row++){
        for(col = row->begin(); col != row->end(); col++){
            std::cout << "Current local grid value is: " << *col << std::endl;
        }
    }

    // Test get_curr_grid_pos
    ROS_INFO("Testing get_curr_grid_pos");
    int* curr_grid_pos;
    curr_grid_pos = map.get_curr_grid_pos(150.29, 75.56);
    ROS_INFO("Current grid position at: (%d, %d)", *curr_grid_pos, *(curr_grid_pos+1));

    // Test get_coord_from_idx and get_idx_from_coord
    ROS_INFO("Testing get_coord_from_idx");
    std::vector<int> coord = map.get_coord_from_idx(1230);
    ROS_INFO("Current idx is %d and cell coord is (%d , %d)", 1230, coord[0], coord[1]);

    ROS_INFO("Testing get_idx_from_coord");
    int curr_idx = map.get_idx_from_coord(coord);
    ROS_INFO("Current coord is (%d, %d), and current idx is: %d", coord[0], coord[1], curr_idx);

    //============================Dijkstra Test Section===========================
    GlobalPathFinder dijkstra_finder("dijkstra", &map);

    // Test path finding function
    ROS_INFO("Testing find_path function");
    std::vector<int> start = {500, 250};
    std::vector<int> goal = {520, 250};
    std::vector<std::vector<int>> path = dijkstra_finder.find_path(start, goal);
    std::vector<std::vector<int>>::iterator it;
    
    ROS_INFO("Start at (%d, %d). Goal at (%d, %d)", start[0], start[1], goal[0], goal[1]);
    ROS_INFO("Found Path: ");
    for(it = path.begin(); it != path.end(); it++){
        ROS_INFO("Path Cell Coordinate: (%d, %d)", (*it)[0], (*it)[1]);
    }
}