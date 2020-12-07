#include <ros/ros.h>
#include <bot_control.h>


int main(int argc, char** argv){
    ros::init(argc, argv, "simple_controller_node");
    ros::NodeHandle n;
    ros::Publisher pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
    bot_control bc;
    bc.init();
    ros::Rate loop_rate(100);
    while(ros::ok()){
        
        bc.move();        
        geometry_msgs::Twist cmd = bc.get_cmd();
        ROS_INFO("cmd_vel: linear: %f, angular: %f", cmd.linear.x, cmd.angular.z);
        pub.publish(cmd);

        ROS_INFO("------------ Spin once ------------");
        ros::spinOnce();
        loop_rate.sleep();
    }
    

}