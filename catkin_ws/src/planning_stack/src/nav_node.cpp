#include "map_reader.h"
#include <ros/ros.h>
#include <iostream>
#include <PotentialField.h>


int main(int argc, char** argv){
    ros::init(argc, argv, "nav_node");
    ros::NodeHandle nh;
    MapReader map(&nh);
    // for(int i=500; i<=800; i++){
    //     ROS_INFO("Current occ value is: %d", map.get_occ_val(i, 250));
    // }
    PotentialField(nav_msgs::Path gp, map);
    



}


