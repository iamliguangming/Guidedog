#include <ros/ros.h>
#include "std_msgs/String.h"
#include <string>
#include <gazebo_msgs/ModelStates.h>
#include <iostream>
#include <vector>

void printRlocation(const gazebo_msgs::ModelStates::ConstPtr &msg){
    std::string name;
    double rx;
    double ry;
    std::vector<std::vector<double> > peds;

    int msg_length = msg->name.size();
    int cnt = 0;
    for (int i=0;i<msg_length;i++)
    { 
        std::string cnt_s = std::to_string(cnt);

        if(msg->name[i] == cnt_s.c_str()){ // only print out the pedetrians' info
            cnt++;
            name = msg->name[i]; 
            rx = msg->pose[i].position.x;
            ry = msg->pose[i].position.y;
            // ROS_INFO("Ped name: %s", name.c_str());
            // ROS_INFO("x = %f, y = %f \n", rx, ry);
            std::vector<double> ped = {rx, ry};
            peds.push_back(ped);
        }
      
    }

    ROS_INFO("Pedestrian positoins:\n");
    ROS_INFO("id      x         y");
    for(int i = 0; i < peds.size(); i++){
        ROS_INFO("%d: %f  %f", i, peds[i][0], peds[i][1]);
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
    ros::Subscriber sub = n.subscribe("/rlocation", 10, rlocationProcessing);

    ros::spin();
    return 0;

}