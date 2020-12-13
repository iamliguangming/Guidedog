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
	
	int cnt = 0;
	// double max_lin_vel = 0.22;
	// double max_ang_vel = 2.84;	
	double max_lin_vel = 0.3;	// max linear speed
	double max_ang_vel = 5.0;	// max angular speed

	double angular_pre = 0.0;
	double angular_window = 100.0;	// ignore

	bool new_info_flag = false;
	double cmd_discount_rate = 0.95; 	// ignore

	double reverse_discount = 2.0;	 // when reversing, move faster
	double emergency_discount = 2.0;	// when in panic (large force), move faster
	double no_force_discount = 0.75;		// the controller is runing at 100Hz but the force signal is running at 10Hz. When no force signal, add discount on the earlier command
	double no_force_scale = 1.0;	


	ros::NodeHandle n;
	std::string odom_topic_name = "/odom";
    ros::Subscriber odom_sub;
    std::string Force_topic_name = "/PFforce";
    ros::Subscriber Force_sub;

	geometry_msgs::Pose2D bot_pos;
	geometry_msgs::Pose2D Force;
	double goal_angle;
	double stop_flag = 1.0;

	geometry_msgs::Twist cmd;

	void Force_call_back(const geometry_msgs::Pose2D::ConstPtr &msg);
	void Odom_call_back(const nav_msgs::Odometry::ConstPtr &msg);
	
};

#endif // BOT_CONTORL
