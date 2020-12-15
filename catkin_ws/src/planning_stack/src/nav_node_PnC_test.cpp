#include "map_reader.h"
#include "global_path_finder.h"
#include <ros/ros.h>
#include <iostream>
#include <vector>
#include "PotentialField.h"


int main(int argc, char** argv){
    ros::init(argc, argv, "nav_node_PnC_test");
    ros::NodeHandle nh;
    MapReader map(&nh);

    //============================Dijkstra Test Section===========================
    // GlobalPathFinder dijkstra_finder(&nh, "dijkstra", &map);

    PotentialField PF(map);
    PF.init();
    PF.run();
    //===========================Test Publish Topic and Visualization===============
    // ROS_INFO("Start Publishing...............");
    // ros::Rate loop_rate(5);
    // while(ros::ok()){
    //     nav_msgs::Path path;
    //     ROS_INFO("Please specify a goal in Rviz ...");
    //     while(1){
    //         path = dijkstra_finder.fireup();
    //         ros::spinOnce();
    //         loop_rate.sleep();
    //         if(path.poses.size() > 1) break;
    //     }
    //     dijkstra_finder.publish_path();
    
    //     // ROS_INFO("Found Path: ");
    //     // for(int i = 0; i < path.poses.size(); i++){
    //     //     ROS_INFO("Path Cell Coordinate: (%.2f, %.2f)", path.poses[i].pose.position.x - 115.0, path.poses[i].pose.position.y - 115.0);
    //     // }

    //     PF.getNewPath(path);
    //     PF.run();
    //     ros::spinOnce();
    //     loop_rate.sleep();
    // }

}
