#include <ros/ros.h>
#include "std_msgs/String.h"
#include <string>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Point32.h>
#include <gazebo_msgs/ModelStates.h>
#include <sensor_msgs/PointCloud.h>
#include <iostream>
#include <cmath>

static double K_p = 1;
class RelativeLocation
{
  private:
  ros::NodeHandle n;
  std::string model_states_topic_name = "/gazebo/model_states";
  std::string odom_topic_name = "/odom";
  ros::Subscriber model_state_sub;
  ros::Subscriber odom_sub;
  ros::Publisher obs_msg_pub;
  ros::Publisher point_cloud_pub;
  double bot_x;
  double bot_y;
  double angle;
  ros::Publisher cmd_vel_pub;
  geometry_msgs::Twist cmd_vel;

  
  public:
  RelativeLocation(int number)
  {   
      int num = number;
      point_cloud_pub = n.advertise<sensor_msgs::PointCloud>("/PointCloud",10);
      obs_msg_pub = n.advertise<gazebo_msgs::ModelStates>("/rlocation",10);
      odom_sub = n.subscribe(odom_topic_name,10,&RelativeLocation::Odom_call_back,this);
      model_state_sub = n.subscribe(model_states_topic_name,10,&RelativeLocation::Model_states_call_back,this);
      cmd_vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel",10);
      cmd_vel.linear.x =  0.1;


  }
  void Model_states_call_back(const gazebo_msgs::ModelStates::ConstPtr &msgs)
{
    int msg_length = msgs->name.size();
    gazebo_msgs::ModelStates relative_location;
    sensor_msgs::PointCloud point_cloud;
    geometry_msgs::Point32 current_point;
    for (int i=0;i<msg_length;i++)
    { 
      current_point.x  = msgs->pose[i].position.x;
      current_point.y  = msgs->pose[i].position.y;
      current_point.z  = msgs->pose[i].position.z;
      point_cloud.points.push_back(current_point);     
      relative_location.name.push_back(msgs->name[i]); // Copying the name of models in the environment
      relative_location.pose.push_back(msgs->pose[i]);
      relative_location.twist.push_back(msgs->twist[i]);
      relative_location.pose[i].position.x = msgs->pose[i].position.x - RelativeLocation::bot_x; //Give Relative obstacle X location wrt Bot
      relative_location.pose[i].position.y = msgs->pose[i].position.y - RelativeLocation::bot_y; //Give Relative obstacle Y location wrt Bot 
      if (msgs->name[i] == "turtlebot3_burger"){
        double angle = msgs->pose[i].orientation.z;
        angle = std::asin(angle);
        cmd_vel.angular.z = - K_p * angle;
        // std::cout << angle << std::endl;
      }
    }
    obs_msg_pub.publish(relative_location);
    // point_cloud_pub.publish(point_cloud);
    // cmd_vel_pub.publish(cmd_vel);
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