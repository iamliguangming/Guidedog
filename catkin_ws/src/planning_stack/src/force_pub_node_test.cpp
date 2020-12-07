#include <ros/ros.h>
#include <geometry_msgs/Pose2D.h>


int main(int argc, char** argv){
    ros::init(argc, argv, "force_pub_node_test");
    ros::NodeHandle n;
    ros::Publisher pub = n.advertise<geometry_msgs::Pose2D>("PFforce", 10);
    ros::Rate loop_rate(10);
    geometry_msgs::Pose2D F;

    while(ros::ok()){
        F.x = 1.0;
        F.y = 1.0;
        F.theta = 0.0;

        ROS_INFO("F: x: %f, y: %f", F.x, F.y);
        
        // ros::spinOnce();
        pub.publish(F);
        loop_rate.sleep();

        // ros::Duration(0.5).sleep();
    }

    return 0;

}