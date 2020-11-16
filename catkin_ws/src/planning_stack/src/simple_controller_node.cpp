#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Point.h>
#include <tf/tf.h>
#include <nav_msgs/Odometry.h>
#include <angles/angles.h>
#include <gazebo_msgs/ModelStates.h>
#include <nav_msgs/Path.h>
#include <geometry_msgs/Pose2D.h>
#include <math.h>
#include <bot_control.h>


int main(int argc, char** argv){
    ros::init(argc, argv, "simple_controller_node");
    ros::NodeHandle n;
    ros::Publisher pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
    bot_control bc;
    bc.init();
    while(ros::ok()){
        bc.move();
        geometry_msgs::Twist cmd = bc.get_cmd();
        ROS_INFO("cmd_vel:\n linear: %f, angular: %f", cmd.linear.x, cmd.angular.z);
        pub.publish(cmd);

    }
    

}