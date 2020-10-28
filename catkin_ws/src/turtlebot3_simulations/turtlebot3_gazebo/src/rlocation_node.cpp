#include <ros/ros.h>
#include "std_msgs/String.h"
#include <string>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Pose.h>
#include <gazebo_msgs/ModelStates.h>

int main(int argc, char **argv)
{

  // ROS_INFO("Using config file: %s", config_file.c_str());
  // ROS_INFO("Using prefix: %s", pub_sub_prefix.c_str());

  // ros::init(argc, argv, pub_sub_prefix_no_slash + "particle_filter");
  ros::init(argc,argv,"rlocation_node");  //Declare the node name
  ROS_INFO("Relative Location Node initiated");
  ros::NodeHandle n;
  std::string model_states_topic_name = "/gazebo/model_states";
  ros::Subscriber model_state_sub;
  ros::Publisher obs_msg_pub;
  // model_state_sub = n.subscribe<gazebo_msgs::ModelState>




  ros::spin();

  return 0;
}