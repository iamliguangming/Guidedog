#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <tf/tf.h>
#include <nav_msgs/Odometry.h>
#include <angles/angles.h>
#include <geometry_msgs/Pose2D.h>
#include <math.h>
#include <cmath>
#include <bot_control.h>

#define pi 3.141592653589793


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
    // ROS_INFO("1, 1, %f", atan2(1, 1) + pi);
    // ROS_INFO("1, -1, %f", atan2(1, -1) + pi);
    // ROS_INFO("-1, 1, %f", atan2(-1, 1) + pi);
    // ROS_INFO("-1, -1, %f", atan2(-1, -1) + pi);
    // ROS_INFO("pi: %f", M_PI);
    ROS_INFO("Force: x:%f, y:%f, angle:%f", Force.x, Force.y, goal_angle);
    ROS_INFO("bot state: x:%f, y:%f, theta:%f", bot_pos.x, bot_pos.y, bot_pos.theta);
    

    if(stop_flag == 0.0){ 
        // if(new_info_flag){
        //     new_info_flag = false;

            double delta_angle = goal_angle - bot_pos.theta;
            ROS_INFO("original delta_angle: %f", delta_angle);
            double delta_angle_ccw = 0.0;
            double delta_angle_cw = 0.0;
            
            if(delta_angle > 0){
                delta_angle_ccw =  delta_angle;
                delta_angle_cw = 2 * pi - delta_angle_ccw;
            }else{
                delta_angle_ccw = 2 * pi + delta_angle;
                delta_angle_cw = 2 * pi - delta_angle_ccw;
            }
            ROS_INFO("CW angle: %f", delta_angle_cw);
            ROS_INFO("CCW angle: %f", delta_angle_ccw);
            // ROS_INFO("modified delta_angle: %f", std::min(delta_angle_ccw, delta_angle_cw));
            ROS_INFO("min angle: %f: ", std::min(delta_angle_ccw, delta_angle_cw));

            if(std::min(delta_angle_ccw, delta_angle_cw) <= 2.0 / 3.0 * pi){
                
                if(delta_angle_ccw < delta_angle_cw){
                    cmd.angular.z = max_ang_vel / M_PI * delta_angle_ccw;  
                    cmd.linear.x = - max_lin_vel / M_PI * abs(delta_angle_ccw) + max_lin_vel;//make linear velocity negatively proportional 
                                                                                    // to the delta_angle
                    ROS_INFO("Turn CCW");
                }else{
                    cmd.angular.z = - max_ang_vel / M_PI * delta_angle_cw; 
                    cmd.linear.x = - max_lin_vel / M_PI * abs(delta_angle_cw) + max_lin_vel;//make linear velocity negatively proportional 
                                                                                    // to the delta_angle
                    ROS_INFO("Turn CW");
                }

            }else{
                double angle_sup = M_PI - std::max(delta_angle_ccw, delta_angle_cw);
                if(delta_angle_ccw < delta_angle_cw){
                    cmd.angular.z = reverse_discount * (max_ang_vel / M_PI * angle_sup);  
                    cmd.linear.x = - reverse_discount * ( - max_lin_vel / M_PI * abs(angle_sup) + max_lin_vel);
                    ROS_INFO("Reverse Turn CW");
                }else{
                    cmd.angular.z = - max_ang_vel / M_PI * angle_sup;  
                    cmd.linear.x = - ( - max_lin_vel / M_PI * abs(angle_sup) + max_lin_vel);
                    ROS_INFO("Reverse Turn CCW");
                }
            }

           
            // cmd.angular.z = cmd.angular.z < (angular_pre + angular_window)?cmd.angular.z:(angular_pre + angular_window);
            // cmd.angular.z = cmd.angular.z > (angular_pre - angular_window)?cmd.angular.z:(angular_pre - angular_window);
            // angular_pre = cmd.angular.z;
            ROS_INFO("modified delta_angle: %f", std::min(delta_angle_ccw, delta_angle_cw));

        // }else{
        //     cmd.linear.x = cmd_discount_rate * cmd.linear.x;
        //     cmd.angular.z =  cmd_discount_rate * cmd.angular.z;
        // }
        
    }else{
        cmd.linear.x = 0;
        cmd.angular.z = 0;
    }

    // ROS_INFO("cmd.linear.x: %f", cmd.linear.x);
    // ROS_INFO("cmd.angular.z: %f", cmd.angular.z);
}

void bot_control::Force_call_back(const geometry_msgs::Pose2D::ConstPtr &msg){                   //message from potential field Force{
    new_info_flag = true;
    Force.x = msg->x;
    Force.y = msg->y;
    stop_flag = msg->theta;
    goal_angle = atan2(Force.y, Force.x);

    goal_angle += pi;
    // ROS_INFO("New force received ---");
    // ROS_INFO("Force: x:%f, y:%f, angle:%f", Force.x, Force.y, goal_angle);
}

//Get current positions
void bot_control::Odom_call_back(const nav_msgs::Odometry::ConstPtr &msg){
    // ROS_INFO("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    new_info_flag = true;
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
    
    yaw += pi;
    bot_pos.theta = yaw;
    // ROS_INFO("New bot pos received ---");
    // ROS_INFO("bot state: x:%f, y:%f, theta:%f", bot_pos.x, bot_pos.y, bot_pos.theta);
}

geometry_msgs::Twist bot_control::get_cmd(){
    return cmd;
}

