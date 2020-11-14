#include "map_reader.h"
#include <ros/ros.h>
#include <iostream>
#include <vector>


int main(int argc, char** argv){
    ros::init(argc, argv, "nav_node");
    ros::NodeHandle nh;
    MapReader map(&nh);

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

}