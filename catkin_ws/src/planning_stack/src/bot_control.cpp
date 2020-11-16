#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <tf/tf.h>
#include <nav_msgs/Odometry.h>
#include <angles/angles.h>
#include <geometry_msgs/Pose2D.h>
#include <math.h>
#include <cmath>
#include <bot_control.h>


bot_control::bot_control(){
    ROS_INFO("Bot control constructor called");
};

void bot_control::init(){
    odom_sub = n.subscribe(odom_topic_name, 10,&bot_control::Odom_call_back,this);    // robot global pose
    Force_sub = n.subscribe(Force_topic_name, 10, &bot_control::Force_call_back, this);  // peds relative pose
    ROS_INFO("--- Controller initialization completed ---");
}

//control and move

void bot_control::move(){
    
    if(stop_flag == 0.0) {
        double delta_angle = goal_angle - bot_pos.theta;
        ROS_INFO("delta_angle: %f", delta_angle);
        cmd.linear.x = - max_lin_vel / M_PI * abs(delta_angle) + max_lin_vel;//make linear velocity negatively proportional 
                                                                            // to the delta_angle
        if(delta_angle >= 0) {
            cmd.angular.z = max_ang_vel / M_PI * delta_angle; //turn ccw +
        }else{
            cmd.angular.z = max_ang_vel / M_PI * delta_angle;//turn cw -
        }
    }else{
        cmd.linear.x = 0;
        cmd.angular.z = 0;
    }

    ROS_INFO("cmd.linear.x: %f", cmd.linear.x);
    ROS_INFO("cmd.angular.z: %f", cmd.angular.z);
}

void bot_control::Force_call_back(const geometry_msgs::Pose2D::ConstPtr &msg){                   //message from potential field Force{
    Force.x = msg->x;
    Force.y = msg->y;
    stop_flag = msg->theta;
    goal_angle = atan2(Force.x, Force.y);
    ROS_INFO("goal_angle: %f", goal_angle);
}

//Get current positions
void bot_control::Odom_call_back(const nav_msgs::Odometry::ConstPtr &msg){
    // ROS_INFO("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    bot_pos.x = msg->pose.pose.position.x;
    bot_pos.y = msg->pose.pose.position.y;

    tf::Quaternion q(
    msg->pose.pose.orientation.x,
    msg->pose.pose.orientation.y,
    msg->pose.pose.orientation.z,
    msg->pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);
    bot_pos.theta = yaw;
    ROS_INFO("bot theta: %f", yaw);
}

geometry_msgs::Twist bot_control::get_cmd(){
    return cmd;
}


