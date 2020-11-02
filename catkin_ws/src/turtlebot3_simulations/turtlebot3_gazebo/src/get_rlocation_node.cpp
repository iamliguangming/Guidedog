#include <ros/ros.h>
#include "std_msgs/String.h"
#include <string>
#include <gazebo_msgs/ModelStates.h>
#include <iostream>

void printRlocation(const gazebo_msgs::ModelStates::ConstPtr &msg){
    std::string name;
    double rx;
    double ry;

    int msg_length = msg->name.size();
    for (int i=0;i<msg_length;i++)
    { 
      name = msg->name[i]; 
      rx = msg->pose[i].position.x;
      ry = msg->pose[i].position.y;
      ROS_INFO("Ped name: %s", name);
      ROS_INFO("x = %f, y = %f \n", rx, ry);
    }

}

void pickRlocationWithinRange(const gazebo_msgs::ModelStates::ConstPtr &msg, int R){
    ROS_INFO("TODO");
}

void pickRlocationWithinBlock(const gazebo_msgs::ModelStates::ConstPtr &msg, int L){
    ROS_INFO("TODO");
}

void rlocationProcessing(const gazebo_msgs::ModelStates::ConstPtr &msg){
    std::string name = "";

    printRlocation(msg);
}


int main(int argc, char** argv){
    ros::init(argc, argv, "get_rlocation_node");
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("rlocation", 10, rlocationProcessing);

    ros::spin();
    return 0;

}