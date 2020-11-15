#include <ros/ros.h>
// #include <gazebo_msgs/ModelStates.h>
// #include <nav_msgs/Path.h>
// #include <geometry_msgs/Twist.h>
#include <tf/tf.h>
// #include <geometry_msgs/Pose2D.h>

#include <PotentialField.h>
// #include <map_reader.h>

#include "std_msgs/String.h"
#include <string>
#include <iostream>
#include <vector>
#include <math.h>


PotentialField::PotentialField(nav_msgs::Path gp, MapReader m):globalPath(gp), map(m){
    ROS_INFO("Potential Field constructor called");
};

// PotentialField::~PotentialField(){}

void PotentialField::init(){
    odom_sub = n.subscribe(odom_topic_name,10,&PotentialField::Odom_call_back,this);    // robot global pose
    rlocation_sub = n.subscribe(relative_location_topic_name, 10, &PotentialField::rlocationProcessing, this);  // peds relative pose
    force_pub = n.advertise<geometry_msgs::Pose2D>(force_cmd_name, 5);   //velocity command
    decision_signal.x = 0.0;
    decision_signal.y = 0.0;
    decision_signal.theta = 0.0;
    force_pub.publish(decision_signal);
    globalPath = 
    getNextWaypoint(1);  //get the first waypoint
    ROS_INFO("--- PF initialization completed ---");
}

void PotentialField::run(){
    ros::Rate r(10);
    ROS_INFO("------------------- PF running -------------------");
    int way_point_num = globalPath.poses.size();
    for(int i = 1; i <= way_point_num; i++){
        ROS_INFO("--------- Current waypoint %d / %d ---------", i, way_point_num);
        int cnt = 0;
        while(ros::ok()){
            ROS_INFO("------ %d ------",cnt++);
            step();     //one step
            if(check()){
                getNextWaypoint(i+1);
                break;
            }
            r.sleep();   // sleep for 0.1s
        }
        if(i == way_point_num){
            ROS_INFO("Navigation Completed!");
        }
    }
}

void PotentialField::step(){
    std::vector<double> Force = calculateForce();
    ROS_INFO("Force: %f, %f", Force[0], Force[1]);
    // generateCmd(Force);
    // modifyCmd();
    // do somethind with the force to output a command;
    decision_signal.x = Force[0];
    decision_signal.y = Force[1];
    if(check()) decision_signal.theta = 1.0;
    else decision_signal.theta = 0.0;
    force_pub.publish(decision_signal);
};

// void PotentialField::modifyCmd(){  
//     double x_ = bot_pos.x + 1.0 / f * decision_signal.linear.x * cos(bot_pos.theta);
//     double y_ = bot_pos.y + 1.0 / f * decision_signal.linear.y * sin(bot_pos.theta);
//     if(map.get_occ_val(x_, y_)){
//         if(map)
//     }
// };

// void PotentialField::generateCmd(std::vector<double> &F){
//     decision_signal.linear.x = 
//     decision_signal.linear.y =

// }

std::vector<double> PotentialField::calculateForce(){
    std::vector<double> F_tot(2, 0.0);

    // Fatt from goal ---
    calcFatt(F_tot);
    ROS_INFO("Fatt: %f, %f", F_tot[0], F_tot[1]);
    // Frep from peds ---
    calcFrep_p(F_tot);
    
    // Frep from walls ---
    calcFrep_w(F_tot);

    return F_tot;
};

void PotentialField::calcFatt(std::vector<double> &F_tot){
    double Fatt_mag = 0.0;
    std::vector<double> F_ori(2, 0.0);
    double att_source_dis = distance(bot_pos, next_wp_pos);

    if(att_source_dis <= att_r){
        Fatt_mag = att_scale * att_source_dis;
    }else{
        Fatt_mag = att_const;
    }
    F_ori[0] = (next_wp_pos.x - bot_pos.x) / att_source_dis;
    F_ori[1] = (next_wp_pos.y - bot_pos.y) / att_source_dis;
    F_tot[0] = Fatt_mag * F_ori[0];
    F_tot[1] = Fatt_mag * F_ori[1];
}

void PotentialField::calcFrep_p(std::vector<double> &F_tot){
    double Frep_mag = 0.0;
    std::vector<double> F_ori(2, 0.0);
    std::vector<int> peds_in_range = pickRlocationWithinRange();    // 1 in range, 0 not in range;
    
    for(int i = 0; i < pedRlocations.size(); i++){
        if(peds_in_range[i]){
            double rep_source_dis = sqrt(pow(pedRlocations[i].x, 2.0) + pow(pedRlocations[i].y, 2.0));
            double close_dis = rep_source_dis - bot_r - ped_r;
            if(close_dis <= rep_r_p){
                Frep_mag = rep_scale_p * (1.0 / close_dis - 1.0 / rep_r_p) * pow(1.0 / close_dis, 2.0);
                F_ori[0] = pedRlocations[i].x / rep_source_dis;
                F_ori[1] = pedRlocations[i].y / rep_source_dis;
                F_tot[0] += Frep_mag * F_ori[0];
                F_tot[1] += Frep_mag * F_ori[1];
                ROS_INFO("--- Frep from peds ---");
                ROS_INFO("Frep ped_%d: %f, %f", i, Frep_mag * F_ori[0], Frep_mag * F_ori[1]);
            }
        }
    }
}

void PotentialField::calcFrep_w(std::vector<double> &F_tot){
    // local_map = map.get_local_map();
    double Frep_mag = 0.0;
    std::vector<double> F_ori(2, 0.0);
    // std::vector<std::vector<bool> > local_map(5, std::vector<bool>(5, true));   // local map
    std::vector<std::vector<bool> > local_map;   // local map  
    int row = local_map.size();
    int col = local_map[0].size();

    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            if(local_map[i][j]){
                double dx = map_resolution * (j - col / 2);
                double dy = map_resolution * (row / 2 - i);
                double rep_source_dis = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
                double close_dis = rep_source_dis - bot_r - wall_r;
                if(close_dis <= rep_r_w){
                    Frep_mag = rep_scale_w * (1.0 / close_dis - 1.0 / rep_r_w) * pow(1.0 / close_dis, 2.0);
                    F_ori[0] = - dx / rep_source_dis;
                    F_ori[1] = - dy / rep_source_dis;
                    F_tot[0] += Frep_mag * F_ori[0];
                    F_tot[1] += Frep_mag * F_ori[1];
                    ROS_INFO("--- Frep from wall ---");
                    ROS_INFO("Frep wall_%d: %f, %f", i, Frep_mag * F_ori[0], Frep_mag * F_ori[1]);
                }
            }
        }
    }
}

// void PotentialField::generateCmd(const std::vector<double> &Force){
    
// }

bool PotentialField::check(){
    double dis = sqrt(pow((bot_pos.x - next_wp_pos.x), 2.0) + pow((bot_pos.y - next_wp_pos.y), 2.0) );
    if(dis <= check_arrive_threshold) return true;
    return false;
};

void PotentialField::getNextWaypoint(const int i){
    next_wp_pos.x = globalPath.poses[i].pose.position.x;
    next_wp_pos.y = globalPath.poses[i].pose.position.y;
}

double PotentialField::distance(const geometry_msgs::Pose2D &a , const geometry_msgs::Pose2D &b){
    return sqrt(pow((a.x - b.x), 2.0) + pow((a.y - b.y), 2.0));
}

bool PotentialField::isInMap(const int &x, const int &y){
    return map.get_occ_val(x, y);
}

std::vector<int> PotentialField::pickRlocationWithinRange(){
    printRlocation();

    int size = pedRlocations.size();
    std::vector<int> peds_in_range(size, 0);
    for(int i = 0; i < size; i++){
        if(sqrt(pow(pedRlocations[i].x, 2.0) + pow(pedRlocations[i].y, 2.0)) <= horizon_r) peds_in_range[i]= 1;
    }
    return peds_in_range;
}

void PotentialField::Odom_call_back(const nav_msgs::Odometry::ConstPtr &msg){
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
}

void PotentialField::rlocationProcessing(const gazebo_msgs::ModelStates::ConstPtr &msg){
    std::string name = "";
    pickRlocation(msg);
    printRlocation();
}

void PotentialField::pickRlocation(const gazebo_msgs::ModelStates::ConstPtr &msg){
    std::string name;
    double rx;
    double ry;
    
    int msg_length = msg->name.size();
    int cnt = 0;
    for (int i=0;i<msg_length;i++)
    { 
        std::string cnt_s = std::to_string(cnt);

        if(msg->name[i] == cnt_s.c_str()){ // only print out the pedetrians' info
            cnt++;
            name = msg->name[i]; 
            rx = msg->pose[i].position.x;
            ry = msg->pose[i].position.y;
            // ROS_INFO("Ped name: %s", name.c_str());
            // ROS_INFO("x = %f, y = %f \n", rx, ry);
            geometry_msgs::Pose2D tmp;
            tmp.x = rx;
            tmp.y = ry;
            pedRlocations.push_back(tmp);
        }
    }
    // ROS_INFO("Pedestrian positions:\n");
    // ROS_INFO("id      x         y");
    // for(int i = 0; i < pedRlocations.size(); i++){
    //     ROS_INFO("%d: %f  %f", i, pedRlocations[i][0], pedRlocations[i][1]);
    // }
}

void PotentialField::printRlocation(){
    ROS_INFO("Pedestrian positions:\n");
    ROS_INFO("id      x         y");
    for(int i = 0; i < pedRlocations.size(); i++){
        ROS_INFO("%d: %f  %f", i, pedRlocations[i].x, pedRlocations[i].y);
    }
}
// 
    // void monitor(){};


