#ifndef POTENTIALFIELD
#define POTENTIALFIELD
#include <ros/ros.h>
#include <gazebo_msgs/ModelStates.h>
#include <nav_msgs/Path.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose2D.h>
#include <nav_msgs/Odometry.h>
#include <map_reader.h>
#include <math.h>

class PotentialField{
    public:
    PotentialField(nav_msgs::Path gp, MapReader m);        
    void init();
    void run();

// ------------------------------------------------------------------------------
    private://paremeters for tuning 
    double check_arrive_threshold = 0.1;    //tolerance
    double att_scale = 1.0;   //Fatt intensity
    double att_r = 2.0;     // parabolic to conic well boundary
    double att_const = att_scale * att_r;   //   
    double rep_scale_p = 1.0;     //Frep intneisty
    double rep_r_p = 0.8;     // radius of the repulsive field

    double horizon_r = 5.0;     // robot vision horizon
    double bot_r = 0.35;     // robot radius
    double ped_r = 0.3;     // ped radius

    double map_resolution = 0.2;
    double rep_scale_w = 1.0;
    double wall_r = map_resolution * sqrt(2.0) / 2;
    double rep_r_w = 0.2;
    

    // ros::Rate freq(10);     // 10 Hz frequency each PF step

    nav_msgs::Path globalPath;
    MapReader map;
    std::vector<geometry_msgs::Pose2D> pedRlocations;   //relative position of the peds to the robot
    geometry_msgs::Pose2D bot_pos;
    geometry_msgs::Pose2D next_wp_pos;      // next waypoint position

    ros::NodeHandle n;
    std::string odom_topic_name = "/odom";
    ros::Subscriber odom_sub;
    std::string relative_location_topic_name = "/rlocation";
    ros::Subscriber rlocation_sub;
    std::string force_cmd_name = "PFforce";
    ros::Publisher force_pub;     // com_vel publisher    
    geometry_msgs::Pose2D decision_signal;
    
    std::string path_topic = "/global_path";
    ros::Publisher path_pub;
    

    void step();        // each PF step
    // void modifyCmd();
    // void generateCmd(std::vector<double> &F);   
    std::vector<double> calculateForce();
    void calcFatt(std::vector<double> &F_tot);
    void calcFrep_p(std::vector<double> &F_tot);
    void calcFrep_w(std::vector<double> &F_tot);
    // void generateCmd(const std::vector<double> &Force);
    bool check();     // check if reaches the next way point
    void getNextWaypoint(const int i);
    double distance(const geometry_msgs::Pose2D &a , const geometry_msgs::Pose2D &b);
    bool isInMap(const int &x, const int &y);
    std::vector<int> pickRlocationWithinRange();    // returns a vectorof 1s and 0s. 1:in range, 0:not in range
    void Odom_call_back(const nav_msgs::Odometry::ConstPtr &msg);
    void rlocationProcessing(const gazebo_msgs::ModelStates::ConstPtr &msg);
    void pickRlocation(const gazebo_msgs::ModelStates::ConstPtr &msg);      // pick the peds from the other objs in the simualtor
    void printRlocation();
};

#endif // POTENTIALFIELD