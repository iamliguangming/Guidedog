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
#include <algorithm>
#include <stdio.h>


PotentialField::PotentialField(MapReader m):map(m){
    ROS_INFO("Potential Field constructor called ... ");
};

// PotentialField::~PotentialField(){}

void PotentialField::init(){
    ROS_INFO("PF initializating ...");
    ros::Rate r_init(10);
    odom_sub = n.subscribe("/odom",1, &PotentialField::Odom_call_back,this);    // robot global pose
    rlocation_sub = n.subscribe("/rlocation", 1, &PotentialField::rlocationProcessing, this);  // peds relative pose
    force_pub = n.advertise<geometry_msgs::Pose2D>(force_cmd_name, 1);   //velocity command
    path_pub = n.advertise<nav_msgs::Path>(path_topic, 1);
    int cnt = 10;
    while(cnt--){
        ros::spinOnce();
        r_init.sleep();
    }
    // getSparseGlobalPath();
    // getNextWaypoint(1);  //get the first waypoint
    ROS_INFO("PF initialization completed. ");
}

void PotentialField::run(){
    ros::Rate r(run_freq);
    ROS_INFO("------------------- PF running -------------------");
    int way_point_num = sparse_global_path.poses.size();
    for(int i = 1; i < way_point_num; i++){
        ROS_INFO("--------------- Current waypoint %d / %d ---------------", i, way_point_num - 1 );
        getNextWaypoint(i);
        int cnt = 0;
        while(ros::ok()){            
            ros::spinOnce();
            r.sleep();   // sleep for 0.1s
            ROS_INFO(" ");
            ROS_INFO("-------------------- WP_%d / %d, iter_%d -------------------",i, way_point_num - 1, cnt++);
            // path_pub.publish(sparse_global_path);
            
            step();     //one step

            if(check()){
                ROS_INFO("WP_%d / %d reached.", i, way_point_num - 1);
                if(i == way_point_num - 1){
                    ROS_INFO("Goal reached ...");
                    decision_signal.x = 0.0;
                    decision_signal.y = 0.0;
                    decision_signal.theta = 1.0;
                    force_pub.publish(decision_signal);
                } 
                break;
            }
        }
    }
    ROS_INFO("End of run ...");
}

void PotentialField::step(){
    std::vector<double> Force = calculateForce();
    ROS_INFO("Force: % f, % f", Force[0], Force[1]);
    
    decision_signal.x = Force[0];
    decision_signal.y = Force[1];
    if(check()){
        decision_signal.theta = 1.0;
    }
    else{
        decision_signal.theta = 0.0;
        if(far_avoid_flag == 2.0) decision_signal.theta = 2.0;
        if(far_avoid_flag == 3.0) decision_signal.theta = 3.0;
    }
    force_pub.publish(decision_signal);
};

std::vector<double> PotentialField::calculateForce(){
    std::vector<double> F_tot(2, 0.0);
    ROS_INFO("bot_pos: % f, % f,  %f", bot_pos.x, bot_pos.y, bot_pos.theta);
    ROS_INFO("next_wp_pos: % f % f", next_wp_pos.x, next_wp_pos.y);

    // Fatt from goal ---
    calcFatt(F_tot);
    // Frep from peds ---
    calcFrep_p(F_tot);
    // Frep from walls ---
    calcFrep_w(F_tot);
    // Frep from danger index
    if(DI) calcDangerIndex(F_tot);
    // Frep from velo ---
    calcFrep_v(F_tot);

    return F_tot;
};

void PotentialField::calcFatt(std::vector<double> &F_tot){
    ROS_INFO("------------- Fatt from WPs --------------");
    std::vector<double> F_ori(2, 0.0);
    double att_source_dis = distance(bot_pos, next_wp_pos);
    double Fatt_mag = 0.0;
    if(att_source_dis <= att_r){
        Fatt_mag = att_scale * att_source_dis;
    }else{
        Fatt_mag = att_const;
    }
    F_ori[0] = (next_wp_pos.x - bot_pos.x) / att_source_dis;
    F_ori[1] = (next_wp_pos.y - bot_pos.y) / att_source_dis;
    F_tot[0] = Fatt_mag * F_ori[0];
    F_tot[1] = Fatt_mag * F_ori[1];
    
    ROS_INFO("Fatt: % f, % f", F_tot[0], F_tot[1]);
}

void PotentialField::calcFrep_p(std::vector<double> &F_tot){
    ROS_INFO("------------- Frep from peds -------------");
    // updatePedInfo();     // print, pick, get velocity
    printRlocation();
    pickRlocationWithinRange();
    getPedVelocity();

    if(int(ped_info.size()) > 0){
        double sin_t = sin(bot_pos.theta);
        double cos_t = cos(bot_pos.theta);
        for(int i = 0; i < int(ped_info.size()); i++){
            if(peds_in_range[i]){
                std::vector<double> F_ori_r(2, 0.0);    // orientation in robot frame
                std::vector<double> F_ori(2, 0.0);      // orientation in gazebo frame
                double rep_source_dis = sqrt(pow(ped_info[i].posex, 2.0) + pow(ped_info[i].posey, 2.0));
                double close_dis = rep_source_dis - bot_r - ped_r;
                double Frep_mag = 0.0;
                ROS_INFO("ped_%s in range: % f  % f", ped_info[i].name.c_str(), ped_info[i].posex, ped_info[i].posey);
                ROS_INFO("close_dis of ped_%d: %f", i, close_dis);
                
                if(ped_info[i].velox < 0.0){    //calculate repulsive force when the ped is moving towards the robot
                    if(0 < close_dis && close_dis <= rep_r_p){
                        Frep_mag = rep_scale_p * (1.0 / close_dis - 1.0 / rep_r_p) * pow(1.0 / close_dis, 1.5);
                        // Frep_mag = rep_scale_p * (1.0 / close_dis - 1.0 / rep_r_p) * sqrt(1.0 / close_dis);
                        F_ori_r[0] = - ped_info[i].posex / rep_source_dis;
                        F_ori_r[1] = - ped_info[i].posey / rep_source_dis;
                        F_ori[0] = F_ori_r[0] * cos_t - F_ori_r[1] * sin_t;
                        F_ori[1] = F_ori_r[0] * sin_t + F_ori_r[1] * cos_t;
                        F_tot[0] += Frep_mag * F_ori[0];
                        F_tot[1] += Frep_mag * F_ori[1];
                        ROS_INFO("Frep ped_%d: % f, % f", i, Frep_mag * F_ori[0], Frep_mag * F_ori[1]);
                    }
                    if(close_dis < 0){
                        Frep_mag = - rep_scale_p * (1.0 / close_dis - 1.0 / rep_r_p) * pow(1.0 / close_dis, 1.5);
                        // Frep_mag = - rep_scale_p * (1.0 / close_dis - 1.0 / rep_r_p) * sqrt(1.0 / close_dis);
                        F_ori_r[0] = - ped_info[i].posex / rep_source_dis;
                        F_ori_r[1] = - ped_info[i].posey / rep_source_dis;
                        F_ori[0] = F_ori_r[0] * cos_t - F_ori_r[1] * sin_t;
                        F_ori[1] = F_ori_r[0] * sin_t + F_ori_r[1] * cos_t;
                        F_tot[0] += Frep_mag * F_ori[0];
                        F_tot[1] += Frep_mag * F_ori[1];
                        ROS_INFO(" Hit  ped_%d: % f, % f !!!!!!!", i, Frep_mag * F_ori[0], Frep_mag * F_ori[1]);
                    }
                }
                
            }
        }
    }  
}

void PotentialField::calcFrep_w(std::vector<double> &F_tot){ 
    ROS_INFO("------------- Frep from wall -------------");
    std::vector<double> F_ori(2, 0.0);
    std::vector<double> xy_double = {bot_pos.x + map_offset_x, bot_pos.y + map_offset_y};
    std::vector<std::vector<bool>> local_map = map.get_local_occ_grid(xy_double, local_map_size);   // local map 

    ROS_INFO("Local map: ");
    // for(int i= 0; i < local_map.size(); i++){
    //     ROS_INFO("%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d%2d", \
    //     int(local_map[i][0]), int(local_map[i][1]), int(local_map[i][2]), int(local_map[i][3]), int(local_map[i][4]), int(local_map[i][5]), int(local_map[i][6]), int(local_map[i][7]), int(local_map[i][8]), int(local_map[i][9]),\
    //     int(local_map[i][10]), int(local_map[i][11]), int(local_map[i][12]), int(local_map[i][13]), int(local_map[i][14]), int(local_map[i][15]), int(local_map[i][16]), int(local_map[i][17]), int(local_map[i][18]), int(local_map[i][19]),\
    //     int(local_map[i][20]), int(local_map[i][21]), int(local_map[i][22]), int(local_map[i][23]), int(local_map[i][24]), int(local_map[i][25]), int(local_map[i][26]), int(local_map[i][27]), int(local_map[i][28]), int(local_map[i][29]), int(local_map[i][30]));
    // }

    int row = local_map.size();
    // ROS_INFO("local map size:%d", row);
    int col = local_map[0].size();
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            if(local_map[i][j]){
                double Frep_mag = 0.0;
                double dx = map_resolution * (j - col / 2);     
                double dy = map_resolution * (row / 2 - i);
                double rep_source_dis = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
                double close_dis = rep_source_dis - bot_r - wall_r;
                if(0 < close_dis && close_dis <= rep_r_w){
                    Frep_mag = rep_scale_w * (1.0 / close_dis - 1.0 / rep_r_w) * pow(1.0 / close_dis, 2.0);
                    F_ori[0] = - dx / rep_source_dis;
                    F_ori[1] = - dy / rep_source_dis;
                    F_tot[0] += Frep_mag * F_ori[0];
                    F_tot[1] += Frep_mag * F_ori[1];
                    ROS_INFO("Frep wall_%d_%d: % f, % f", i,j, Frep_mag * F_ori[0], Frep_mag * F_ori[1]);
                }
                if(close_dis < 0){
                    Frep_mag = - rep_scale_w * (1.0 / close_dis - 1.0 / rep_r_w) * pow(1.0 / close_dis, 2.0);
                    F_ori[0] = - dx / rep_source_dis;
                    F_ori[1] = - dy / rep_source_dis;
                    F_tot[0] += Frep_mag * F_ori[0];
                    F_tot[1] += Frep_mag * F_ori[1];
                    ROS_INFO("Hit wall_%d_%d: % f, % f !!!", i,j, Frep_mag * F_ori[0], Frep_mag * F_ori[1]);
                } 
            }
        }
    }
}


void PotentialField::calcDangerIndex(std::vector<double> &F_tot){
    ROS_INFO("------------- Danger Index --------------");
    printRlocation();
    getPedVelocity();
    // std::vector<int> peds_in_range = pickRlocationWithinRange();
    for(int i = 0; i < ped_info.size(); i++){
        if(peds_in_range[i]){
            double rep_source_dis = sqrt(pow(ped_info[i].posex, 2.0) + pow(ped_info[i].posey, 2.0));
            double ped_vel_x = bot_vel.x + ped_info[i].velox;
            double ped_vel_y = bot_vel.y + ped_info[i].veloy;
            // the distance influence factor ---
            double fcm = 0.0;
            if(rep_source_dis <= rhocmax) fcm = eta * (1 / rep_source_dis - 1 / rhocmax); 
            // the speed influence factor
            double ko = epsilon * sqrt(pow(ped_vel_x, 2.0) + pow(ped_vel_y, 2.0)) - sqrt(pow(bot_vel.x, 2.0) + pow(bot_vel.y, 2.0));
            // the danger index ---
            double v_m_x = bot_vel.x - fcm * ped_vel_x;
            double v_m_y = bot_vel.y - fcm * ped_vel_y;
            double v_p_x = bot_vel.x + fcm * ped_vel_x;
            double v_p_y = bot_vel.y + fcm * ped_vel_y;
            double alpha1 = atan2(v_m_x, v_m_y);
            double beta1 = atan2(v_p_x, v_p_y);
            double angle_bot_to_ped = atan2(ped_info[i].posex, ped_info[i].posey);
            double alpha = abs(alpha1 - angle_bot_to_ped);
            double beta = abs(beta1 - angle_bot_to_ped);
            if(ko > 0 && alpha > 0 && alpha < 1.5708 && fcm > 0){
                F_tot[0] += DI_scale * v_m_x;
                F_tot[1] += DI_scale * v_m_y;
            }else if(ko <= 0 && beta > 0 && beta < 1.5708 && fcm > 0){
                F_tot[0] += DI_scale * v_p_x;
                F_tot[1] += DI_scale * v_p_y;
            }else{
                F_tot[0] += 0.0;
                F_tot[1] += 0.0;
            }

        }
    }
}

void PotentialField::calcFrep_v(std::vector<double> &F_tot){
    ROS_INFO("------------- Frep from velo --------------");

    double sin_t = sin(bot_pos.theta);
    double cos_t = cos(bot_pos.theta);
    // printRlocation();
    getPedVelocity();
    far_avoid_flag = 0.0;
    for(int i = 0; i < ped_info.size(); i++){
        if(peds_in_range[i]){
            std::vector<double> F_ori_r(2, 0.0);    // orientation in robot frame
            std::vector<double> F_ori(2, 0.0);      // orientation in gazebo frame
            double rep_source_dis = sqrt(pow(ped_info[i].posex, 2.0) + pow(ped_info[i].posey, 2.0));
            double v_angle = 0;
            double p_angle = 0;
            // if(rep_source_dis <= rep_r_v){
                double Frep_mag = rep_scale_v * sqrt(pow(ped_info[i].velox, 2.0) + pow(ped_info[i].velox, 2.0));
                ROS_INFO("p in r velox: %f, veloy: %f", ped_info[i].velox, ped_info[i].veloy);
                // ROS_INFO("p in r posex: %f, posey: %f", ped_info[i].posex, ped_info[i].posey);
                v_angle = atan2(- ped_info[i].veloy, - ped_info[i].velox);
                p_angle = atan2(ped_info[i].posey, ped_info[i].posex);
                // ROS_INFO("ped %d", i);
                // ROS_INFO("v_angle: %f", v_angle);
                // ROS_INFO("p_angle: %f", p_angle); 
                double delta_angle = abs(v_angle - p_angle);
                double delta_angle_sup = 2 * pi - delta_angle;
                // ROS_INFO("delta angle: %f", delta_angle);
                // ROS_INFO("delta angle sup %f", delta_angle_sup);
                if(std::min(delta_angle, delta_angle_sup) < 1.57){
                    F_ori[0] = - ped_info[i].posex / rep_source_dis;
                    F_ori[1] = - ped_info[i].posey / rep_source_dis;
                    // F_ori_r[0] = F_ori_r[0] * cos_t - F_ori_r[1] * sin_t;
                    // F_ori_r[1] = F_ori_r[0] * sin_t + F_ori_r[1] * cos_t;
                    // F_tot[0] += Frep_mag * F_ori[0];
                    // F_tot[1] += Frep_mag * F_ori[1];
                    // ROS_INFO("Frep velo_ped_%d: % f, % f", i, Frep_mag * F_ori[0], Frep_mag * F_ori[1]);
                }
            // }

            // when the ped is far and the the robot is going straight to the robot, turn accoding to the sign of (v_angle - p_angle)
            if(rep_source_dis > 1.0 && rep_source_dis < 4.0){
                // if(v_angle - p_angle >= 0 && v_angle - p_angle < 0.15){
                if(p_angle > 0.0 && p_angle < 0.25){
                    ROS_INFO("Ped in front, turn CW");
                    far_avoid_flag = 2.0;
                }
                // else if(v_angle - p_angle < 0 && v_angle - p_angle > -0.15){
                else if(p_angle <= 0.0 && p_angle > -0.25){
                    ROS_INFO("Ped in front, turn CCW");
                    far_avoid_flag = 3.0;  
                } 
                // else{
                //     far_avoid_flag = 0.0;
                // } 
            }else{
                far_avoid_flag = 0.0;
            }
        }
    }
}

bool PotentialField::check(){
    double dis = sqrt(pow((bot_pos.x - next_wp_pos.x), 2.0) + pow((bot_pos.y - next_wp_pos.y), 2.0));
    if(dis <= check_arrive_threshold) return true;
    return false;
};

void PotentialField::getNextWaypoint(const int i){
    next_wp_pos.x = sparse_global_path.poses[i].pose.position.x - map_offset_x;
    next_wp_pos.y = sparse_global_path.poses[i].pose.position.y - map_offset_y;
}

double PotentialField::distance(const geometry_msgs::Pose2D &a , const geometry_msgs::Pose2D &b){
    return sqrt(pow((a.x - b.x), 2.0) + pow((a.y - b.y), 2.0));
}

bool PotentialField::isInMap(const int &x, const int &y){
    std::vector<double> temp = {double(x), double(y)};
    return map.get_occ_val(temp);
}

void PotentialField::Odom_call_back(const nav_msgs::Odometry::ConstPtr &msg){
    // ROS_INFO("in odom_call_back");
    bot_pos.x = msg->pose.pose.position.x;
    bot_pos.y = msg->pose.pose.position.y;
    bot_vel.x = msg->twist.twist.linear.x;
    bot_vel.y = msg->twist.twist.linear.y;

    tf::Quaternion q(
    msg->pose.pose.orientation.x,
    msg->pose.pose.orientation.y,
    msg->pose.pose.orientation.z,
    msg->pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);

    // bot_pos.theta = yaw + 1.57;
    bot_pos.theta = yaw;
    
}

void PotentialField::rlocationProcessing(const gazebo_msgs::ModelStates::ConstPtr &msg){
    // ROS_INFO("in rlocation_call_back");
    pickRlocation(msg);
    // pickRlocationWithinRange();
}

void PotentialField::pickRlocation(const gazebo_msgs::ModelStates::ConstPtr &msg){
    std::vector<planning_stack::ped> ped_info_n;
    std::vector<planning_stack::ped> ped_info_;
    int msg_length = msg->name.size();

    for (int i=0;i<msg_length;i++){ 
        if(msg->name[i].find("man") != std::string::npos){ // only print out the pedetrians' info
            std::string name = msg->name[i]; 
            double rx = msg->pose[i].position.x;
            double ry = msg->pose[i].position.y;
            // ROS_INFO("Ped name: %s", name.c_str());
            // ROS_INFO("x = %f, y = %f \n", rx, ry);

            // new data structure to store the ped infomation ---
            planning_stack::ped tmp2;
            tmp2.name = msg->name[i]; 
            // tmp2.posex = rx;    // for testing planning module, this pose and pose is in gazebo frame
            // tmp2.posey = ry;
            tmp2.posex = rx; // for merging, this posex and posey is in robot frame
            tmp2.posey = -ry;
            tmp2.velox = 0.0;
            tmp2.veloy = 0.0;
            ped_info_n.push_back(tmp2);

            // insert the ped's name in a set
            ped_set.insert(msg->name[i]);
        }
    }
    
    // find out which camera's info is being processed, and put the info in the corresponding ped_info_#
    // if(ped_info_n.size()){
        if(msg->name[0][0] == '1'){     //camera in the front
            if(ped_info_n.size()) ROS_INFO("1");
            ped_info_1 = ped_info_n;
        }else if(msg->name[0][0] == '2'){    //camera on the left
            if(ped_info_n.size()) ROS_INFO("2");
            for(int i = 0; i < ped_info_n.size(); i++){     
                double tmpx = ped_info_n[i].posex * cos_cam - ped_info_n[i].posey * sin_cam;
                double tmpy = ped_info_n[i].posex * sin_cam + ped_info_n[i].posey * cos_cam;
                ped_info_n[i].posex = tmpx;
                ped_info_n[i].posey = tmpy;
            }
            ped_info_2 = ped_info_n;
        }else if(msg->name[0][0] == '3'){       //camera on the right
            if(ped_info_n.size()) ROS_INFO("3");
            for(int i = 0; i < ped_info_n.size(); i++){     //camera on the left
                double tmpx = ped_info_n[i].posex * cos_cam + ped_info_n[i].posey * sin_cam;
                double tmpy = - ped_info_n[i].posex * sin_cam + ped_info_n[i].posey * cos_cam;
                ped_info_n[i].posex = tmpx;
                ped_info_n[i].posey = tmpy;
            }
            ped_info_3 = ped_info_n;
        }

        //update the ped_info whenever a new camera's info comes in 
        ped_info_.reserve(ped_info_1.size() + ped_info_2.size() + ped_info_3.size()); // preallocate memory
        ped_info_.insert(ped_info_.end(), ped_info_1.begin(), ped_info_1.end());
        ped_info_.insert(ped_info_.end(), ped_info_2.begin(), ped_info_2.end());
        ped_info_.insert(ped_info_.end(), ped_info_3.begin(), ped_info_3.end());

        ped_info_pre = ped_info;    //store previous ped_info
        ped_info = ped_info_;   //update ped_info
    // }
    // else{      // not man detected, need to find out which camera 
        
    // }
    // // else if(msg_length == 1 && msg->name[0].find("nan") != std::string::npos){     // only one msg and the name is "nan"
    // //     if(msg->name[0].name[0] == '1') ped_info_1 = ped_info_n;    // ped_info_n is empty
    // //     else if(msg->name[0].name[0] == '2') ped_info_2 = ped_info_n; 
    // //     else if(msg->name[0].name[0] == '3') ped_info_3 = ped_info_n; 
    // // }
}

void PotentialField::pickRlocationWithinRange(){
    std::vector<int> peds_in_range_;
    // printRlocation();

    int size = int(ped_info.size());
    for(int i = 0; i < size; i++){
        if(sqrt(pow(ped_info[i].posex, 2.0) + pow(ped_info[i].posey, 2.0)) <= horizon_r){
            // ROS_INFO("ped name in horizon: %s", (ped_info[i].name).c_str());
            peds_in_range_.push_back(1);
        }else{
            peds_in_range_.push_back(0);
        } 
    }
    peds_in_range = peds_in_range_;
}

void PotentialField::printRlocation(){
    // ROS_INFO("Ped num: %lu", ped_info.size());
    ROS_INFO("Pedestrian positions:");
    ROS_INFO("id      x         y");

    for(int i = 0; i < int(ped_info.size()); i++){
        ROS_INFO("%s: % f  % f", (ped_info[i].name).c_str(), ped_info[i].posex, ped_info[i].posey);
    }
}

void PotentialField::getPedVelocity(){
    std::set<std::string>::iterator iter;

    // for(iter = ped_set.begin() ; iter != ped_set.end() ; ++iter)
    // {
    //     // ROS_INFO("ped name in get velo: %s", (*iter).c_str());
    // }
    for(int i = 0; i < ped_info.size(); i++){
        if(ped_info_pre.size() > 0 && ped_set_pre.find(ped_info[i].name) != ped_set_pre.end()){
            ped_info[i].velox = clamp((ped_info[i].posex - ped_info_pre[i].posex) / (1.0 / run_freq));
            ped_info[i].veloy = clamp((ped_info[i].posey - ped_info_pre[i].posey) / (1.0 / run_freq));
            // ROS_INFO("ped %s relative velocity: % f % f", (ped_info[i].name).c_str(), ped_info[i].velox, ped_info[i].veloy);
        }
    } 

    ped_set_pre = ped_set;
    ped_set.clear();
}

void PotentialField::updatePedInfo(){
    printRlocation();
    pickRlocationWithinRange();
    getPedVelocity();
}

double PotentialField::clamp(double value){
    value = value > 5.0 ? 5.0 : value;
    value = value < -5.0 ? -5.0 : value;
    return value;
}

void PotentialField::newPedMsgTest(){
    ped_trial.name = "ped trial 1";
    ped_trial.posex = 1.0;
    ped_trial.posey = 2.0;
    ped_trial.velox = 3.0;
    ped_trial.veloy = 4.0;
    ROS_INFO("ped name:%s", ped_trial.name.c_str());
    ROS_INFO("ped posex: %f", ped_trial.posex);
    ROS_INFO("ped posey: %f", ped_trial.posey);
    ROS_INFO("ped velox: %f", ped_trial.velox);
    ROS_INFO("ped veloy: %f", ped_trial.veloy);
}

void PotentialField::getSparseGlobalPath(){
    int size = dense_global_path.poses.size();
    geometry_msgs::PoseStamped tmp;
    nav_msgs::Path sparse_global_path_;
    tmp.pose.position.x = dense_global_path.poses[0].pose.position.x;
    tmp.pose.position.y = dense_global_path.poses[0].pose.position.y;
    sparse_global_path_.poses.push_back(tmp);
    int cnt = 0;
    for(int i = 1; i < size - 1; i++){
        cnt++;
        double dx1 = dense_global_path.poses[i].pose.position.x - dense_global_path.poses[i-1].pose.position.x; // i-1 used to be curr_wp
        double dy1 = dense_global_path.poses[i].pose.position.y - dense_global_path.poses[i-1].pose.position.y;
        double dx2 = dense_global_path.poses[i+1].pose.position.x - dense_global_path.poses[i-1].pose.position.x;
        double dy2 = dense_global_path.poses[i+1].pose.position.y - dense_global_path.poses[i-1].pose.position.y;       
        if(abs(dy2 / dx2 - dy1 / dx1) > 0.001){
            cnt = 0;
            ROS_INFO("new WP!  delta slope: %f", abs(dy2 / dx2 - dy1 / dx1));
            tmp.pose.position.x = dense_global_path.poses[i+1].pose.position.x;
            tmp.pose.position.y = dense_global_path.poses[i+1].pose.position.y;
            sparse_global_path_.poses.push_back(tmp);
            i++;
        }
        if(cnt >= 300){
            cnt = 0;
            ROS_INFO("new WP! Each 8 meters");
            tmp.pose.position.x = dense_global_path.poses[i+1].pose.position.x;
            tmp.pose.position.y = dense_global_path.poses[i+1].pose.position.y;
            sparse_global_path_.poses.push_back(tmp);
            i++;
        }
    }
    tmp.pose.position.x = dense_global_path.poses[size - 1].pose.position.x;
    tmp.pose.position.y = dense_global_path.poses[size - 1].pose.position.y;
    sparse_global_path_.poses.push_back(tmp);

    for(int i = 0; i < sparse_global_path_.poses.size(); i++){
        ROS_INFO("Sparse Coordinate: (%.2f, %.2f)", sparse_global_path_.poses[i].pose.position.x -map_offset_y , sparse_global_path_.poses[i].pose.position.y - map_offset_y);
    }
    sparse_global_path = sparse_global_path_;
}

void PotentialField::getNewPath(nav_msgs::Path new_path){
    dense_global_path = new_path;
    getSparseGlobalPath();
    // getNextWaypoint(1);
}