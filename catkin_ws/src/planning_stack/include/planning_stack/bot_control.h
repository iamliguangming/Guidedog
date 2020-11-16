#ifndef BOT_CONTORL
#define BOT_CONTORL
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <angles/angles.h>
#include <geometry_msgs/Pose2D.h>
#include <math.h>

class bot_control{

	public:
	
	bot_control();
	void init();
	void move();
	geometry_msgs::Twist get_cmd();



	private://parameter for conrolling the linear and angular speed
	
	// double max_lin_vel = 0.22;
	// double max_ang_vel = 2.84;	
	double max_lin_vel = 0.22;
	double max_ang_vel = 4.0;

	ros::NodeHandle n;
	std::string odom_topic_name = "/odom";
    ros::Subscriber odom_sub;
    std::string Force_topic_name = "/PFforce";
    ros::Subscriber Force_sub;

	geometry_msgs::Pose2D bot_pos;
	geometry_msgs::Pose2D Force;
	double goal_angle;
	double stop_flag = 0.0;

	geometry_msgs::Twist cmd;

	void Force_call_back(const geometry_msgs::Pose2D::ConstPtr &msg);
	void Odom_call_back(const nav_msgs::Odometry::ConstPtr &msg);
	
};

#endif // BOT_CONTORL
