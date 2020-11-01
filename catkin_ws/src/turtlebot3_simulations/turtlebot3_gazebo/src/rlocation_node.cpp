#include <ros/ros.h>
#include "std_msgs/String.h"
#include <string>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Pose.h>
#include <gazebo_msgs/ModelStates.h>
#include <iostream>

class RelativeLocation
{
  private:
  ros::NodeHandle n;
  std::string model_states_topic_name = "/gazebo/model_states";
  std::string odom_topic_name = "/odom";
  ros::Subscriber model_state_sub;
  ros::Subscriber odom_sub;
  ros::Publisher obs_msg_pub;
  double bot_x;
  double bot_y;
  
  public:
  RelativeLocation(int number)
  {   
      int num = number;
      obs_msg_pub = n.advertise<gazebo_msgs::ModelStates>("/rlocation",10);
      odom_sub = n.subscribe(odom_topic_name,10,&RelativeLocation::Odom_call_back,this);
      model_state_sub = n.subscribe(model_states_topic_name,10,&RelativeLocation::Model_states_call_back,this);

  }

  void Model_states_call_back(const gazebo_msgs::ModelStates::ConstPtr &msgs)
{
    int msg_length = msgs->name.size();
    gazebo_msgs::ModelStates relative_location;
    for (int i=0;i<msg_length;i++)
    { 
      relative_location.name.push_back(msgs->name[i]); // Copying the name of models in the environment
      relative_location.pose.push_back(msgs->pose[i]);
      relative_location.twist.push_back(msgs->twist[i]);
      relative_location.pose[i].position.x = msgs->pose[i].position.x - RelativeLocation::bot_x; //Give Relative obstacle X location wrt Bot
      relative_location.pose[i].position.y = msgs->pose[i].position.y - RelativeLocation::bot_y; //Give Relative obstacle Y location wrt Bot 
    }
    obs_msg_pub.publish(relative_location);
}

void Odom_call_back(const nav_msgs::Odometry::ConstPtr &msgs)
{
    RelativeLocation::bot_x = msgs->pose.pose.position.x;
    RelativeLocation::bot_y = msgs->pose.pose.position.y;
}


};


int main(int argc, char **argv)
{
  ros::init(argc,argv,"rlocation_node");  //Declare the node name
  ROS_INFO("Relative Location Node initiated");
  int num = 1;
  RelativeLocation rlocation(num); 
  
  ros::spin();

  return 0;
}