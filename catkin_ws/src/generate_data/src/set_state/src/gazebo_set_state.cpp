//example pgm to set a model state in Gazebo
// could also do w/ rosservice call gazebo/set_model_state
#include <ros/ros.h> //ALWAYS need to include this
//#include <gazebo_msgs/SetModelState.h>
#include <gazebo_msgs/ModelState.h>
#include <gazebo_msgs/SetModelState.h>
#include <gazebo_msgs/SpawnModel.h>
#include <gazebo_msgs/DeleteModel.h>
#include <string.h>
#include <stdio.h>  
#include <std_msgs/Float64.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/filesystem.hpp>
#include <std_srvs/Empty.h>

#define PI 3.1415926


static int IMG_ID = 0;
static std::vector<std::string> curr_obstacle_arr;

void delete_agent_models(gazebo_msgs::DeleteModel model_delete_msg, ros::ServiceClient delete_model_client, int& counter)
{
    for (int i = 0 ;i<=23;i++){
        // ros::Duration(0.1).sleep();
        model_delete_msg.request.model_name = "man"+std::to_string(i+counter*24);
        ROS_INFO_STREAM("Deleting Model"<<model_delete_msg.request.model_name<<std::endl);
        delete_model_client.call(model_delete_msg);
        bool del_result = model_delete_msg.response.success;
        if(!del_result){
        ROS_WARN("agent model delete error occurs once");}

    }
}

void get_add_message(gazebo_msgs::SpawnModel& model_add_msg, int idx, std::string xml, std::string o_name, std::vector<float> &location_limit, float theta)
{
    std::string model_name = "model_"  + std::to_string(idx);
    model_add_msg.request.model_name = model_name;
    xml.replace(xml.find(o_name),o_name.length(),model_name);
    model_add_msg.request.model_xml = xml;
    float x = std::rand() % int(location_limit[2] - location_limit[0]) + location_limit[0];
    float y = std::rand() % int(location_limit[3] - location_limit[1]) + location_limit[1];

    // std::cout << oZ << std::endl;
    model_add_msg.request.initial_pose.position.x = x;
    model_add_msg.request.initial_pose.position.y = y;
    model_add_msg.request.initial_pose.position.z = -0.75;
    model_add_msg.request.initial_pose.orientation.x = 0;
    model_add_msg.request.initial_pose.orientation.y = 0;
    model_add_msg.request.initial_pose.orientation.z = std::sin(theta);
    model_add_msg.request.initial_pose.orientation.w = std::cos(theta);
}

void get_change_msg(gazebo_msgs::SetModelState& get_change_msg, float x, float y, float theta){
    get_change_msg.request.model_state.model_name = "turtlebot3_burger";
    get_change_msg.request.model_state.pose.position.x = x;
    get_change_msg.request.model_state.pose.position.y = y;
    get_change_msg.request.model_state.pose.position.z = 0;
    
    get_change_msg.request.model_state.pose.orientation.x = 0.0;
    get_change_msg.request.model_state.pose.orientation.y = 0.0;
    get_change_msg.request.model_state.pose.orientation.z = std::sin(theta);
    get_change_msg.request.model_state.pose.orientation.w = std::cos(theta);

}

void add_static_building(int num_static, std::vector<std::string>& static_xml_lib, std::vector<std::string>& static_ori_name_lib, 
                            ros::ServiceClient& spawn_model_client){
    gazebo_msgs::SpawnModel static_add_msg;
    double random_y_shift = (std::rand() % 100 - 20) / 99.0 * 5;
    
    for(int i=0; i<num_static; i++){
        
        int temp_idx = std::rand() % static_xml_lib.size();
        std::string model_name = "building_"  + std::to_string(i);
        static_add_msg.request.model_name = model_name;
        std::string xml = static_xml_lib[temp_idx];
        std::string o_name = static_ori_name_lib[temp_idx];
        // ROS_INFO("I'm Here");
        // ROS_INFO_STREAM(xml);
        xml.replace(xml.find(o_name),o_name.length(),model_name);
        static_add_msg.request.model_xml = xml;

        static_add_msg.request.initial_pose.position.x = (i / 2) * 10 + random_y_shift;
        static_add_msg.request.initial_pose.position.y = std::pow(-1, i+1) * 10 + 5;
        static_add_msg.request.initial_pose.position.z = 0;
        static_add_msg.request.initial_pose.orientation.x = 0;
        static_add_msg.request.initial_pose.orientation.y = 0;
        static_add_msg.request.initial_pose.orientation.z =  std::sin(std::pow(-1, i) * PI/4 + PI/4);
        static_add_msg.request.initial_pose.orientation.w =  std::cos(PI/4 + std::pow(-1, i) * PI/4);


        // cout << model_add_msg.request.model_xml;
        spawn_model_client.call(static_add_msg);
        //make sure service call was successful
        bool add_result = static_add_msg.response.success;
        if (!add_result){
            ROS_WARN("generate error occurs once");
        }
    }
}

void add_static_obstacles(int num_static, std::vector<std::string>& static_xml_lib, std::vector<std::string>& static_ori_name_lib, 
                            ros::ServiceClient& spawn_model_client){
    gazebo_msgs::SpawnModel static_add_msg;
    double random_y_shift = (std::rand() % 100 - 20) / 99.0 * 5;
    
    for(int i=0; i<num_static; i++){
        
        int temp_idx = std::rand() % static_xml_lib.size();
        std::string xml = static_xml_lib[temp_idx];
        std::string o_name = static_ori_name_lib[temp_idx];
        std::string model_name = o_name + std::to_string(i);
        curr_obstacle_arr.push_back(model_name);
        static_add_msg.request.model_name = model_name;
        // ROS_INFO("I'm Here");
        // ROS_INFO_STREAM(xml);
        xml.replace(xml.find(o_name),o_name.length(),model_name);
        static_add_msg.request.model_xml = xml;

        static_add_msg.request.initial_pose.position.x = (i / 2) * 3 + random_y_shift;
        static_add_msg.request.initial_pose.position.y = std::pow(-1, i+1) * 3.5 + 5;
        static_add_msg.request.initial_pose.position.z = 0;
        static_add_msg.request.initial_pose.orientation.x = 0;
        static_add_msg.request.initial_pose.orientation.y = 0;
        static_add_msg.request.initial_pose.orientation.z =  std::sin(-PI/4);
        static_add_msg.request.initial_pose.orientation.w =  std::cos(-PI/4 );


        // cout << model_add_msg.request.model_xml;
        spawn_model_client.call(static_add_msg);
        //make sure service call was successful
        bool add_result = static_add_msg.response.success;
        if (!add_result){
            ROS_WARN("generate error occurs once");
        }
    }
}

void modle_define(std::vector<std::string>& input_xml, std::vector<std::string>& input_name, std::vector<std::string>& xml_lib, std::vector<std::string>& ori_name_lib){
    int length = input_xml.size();
    for (int i=0; i<length; i++){
        std::ifstream t(input_xml[i]);
        std::stringstream buffer;
        buffer << t.rdbuf(); 
        xml_lib.push_back(buffer.str());
        ori_name_lib.push_back(input_name[i]);
    }
}
void imageCb(const sensor_msgs::ImageConstPtr& msg)
{ 
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }
    static int image_count = 0; 
    std::stringstream sstream;                               // added this
    sstream << "src/generate_data/data/label/my_image" << IMG_ID << ".png" ;                  // added this
    ROS_ASSERT( cv::imwrite( sstream.str(),  cv_ptr->image ) );      // added this
    // ros::Duration(1).sleep();
}


int main(int argc, char **argv) {
    ros::init(argc, argv, "init_model_state");
    ros::NodeHandle nh;
    ros::Duration half_sec(0.5);
    
    // make sure service is available before attempting to proceed, else node will crash
    bool service_ready = false;
    while (!service_ready) {
    //   service_ready = ros::service::exists("/gazebo/set_model_state",true);
      service_ready = ros::service::exists("/gazebo/spawn_sdf_model",true);
      ROS_INFO("waiting for set_model_state service");
      half_sec.sleep();
    }
    ROS_INFO("set_model_state service exists");

    // model add client
    ros::ServiceClient spawn_model_client = 
        nh.serviceClient<gazebo_msgs::SpawnModel>("/gazebo/spawn_sdf_model");

    // model delete client 
    ros::ServiceClient delete_model_client = 
        nh.serviceClient<gazebo_msgs::DeleteModel>("/gazebo/delete_model");


    ros::ServiceClient set_model_state_client = 
       nh.serviceClient<gazebo_msgs::SetModelState>("/gazebo/set_model_state");

    ros::ServiceClient clear_pedsim_agent = 
        nh.serviceClient<std_srvs::Empty>("/pedsim_simulator/clear_simulation");
    ros::ServiceClient restart_pedsim_agent = nh.serviceClient<std_srvs::Empty>("/pedsim_simulator/restart_simulation");
    
    std_srvs::Empty place_holder_msg;
    // model add msg
    gazebo_msgs::SpawnModel model_add_msg;
    
    // model delete msg
    gazebo_msgs::DeleteModel model_delete_msg;

    // model change msg
    gazebo_msgs::SetModelState model_state_msg;


    std::vector<std::string> mobile_xml_lib;
    std::vector<std::string> mobile_ori_name_lib;
    std::vector<std::string> static_xml_lib;
    std::vector<std::string> static_ori_name_lib;
    std::vector<std::string> static_obstacle_xml_lib;
    std::vector<std::string> static_obstacle_ori_name_lib;


    std::vector<std::string> input_mobile_xml;
    std::vector<std::string> input_mobile_name;
    std::vector<std::string> input_static_obstacle_xml;
    std::vector<std::string> input_static_obstacle_name;
    std::vector<std::string> input_static_xml;
    std::vector<std::string> input_static_name;
    std::vector<std::vector<float>> mobile_scale;

    
    //mobile model sdf define
    input_mobile_xml.push_back("src/generate_data/models/actor_model.sdf");
    input_mobile_name.push_back("actor_model");
    mobile_scale.push_back({0.5, 0.5, 1.5});

    //static model sdf define
    input_static_xml.push_back("src/generate_data/models/house_1/model-1_3.sdf");
    input_static_name.push_back("house_1");
    input_static_xml.push_back("src/generate_data/models/law_office/model.sdf");
    input_static_name.push_back("law_office");
    input_static_xml.push_back("src/generate_data/models/post_office/model.sdf");
    input_static_name.push_back("post_office");
    input_static_obstacle_xml.push_back("src/generate_data/models/fire_hydrant/model.sdf");
    input_static_obstacle_name.push_back("fire_hydrant");
    input_static_obstacle_xml.push_back("src/generate_data/models/first_2015_trash_can/model.sdf");
    input_static_obstacle_name.push_back("trash_can");
    input_static_obstacle_xml.push_back("src/generate_data/models/oak_tree/model.sdf");
    input_static_obstacle_name.push_back("oak_tree");
    input_static_obstacle_xml.push_back("src/generate_data/models/stop_sign/model.sdf");
    input_static_obstacle_name.push_back("stop_sign");
    input_static_obstacle_xml.push_back("src/generate_data/models/mailbox/model.sdf");
    input_static_obstacle_name.push_back("mailbox");
   input_static_obstacle_xml.push_back("src/generate_data/models/stop_light/model.sdf");
    input_static_obstacle_name.push_back("stop_light");



    modle_define(input_mobile_xml, input_mobile_name, mobile_xml_lib, mobile_ori_name_lib);
    modle_define(input_static_xml, input_static_name, static_xml_lib, static_ori_name_lib);
    modle_define(input_static_obstacle_xml,input_static_obstacle_name, static_obstacle_xml_lib, static_obstacle_ori_name_lib);
       
    ros::Rate loop_rate(10);

    // generate parameters
    int num_static = 6;
    int num_static_obstacle = 20;
    int max_num_mobile = 10;
    int robot_state_number = 5;
    int robot_angle_number = 3;


    int past_model_number = 0;
    int current_model_number = 0;
    
    
    // location limit
    std::vector<float> location_limit_p(4);
    location_limit_p = {2, 1, 10, 5};

    std::vector<float> location_limit_robot(4);
    location_limit_robot = {5, 2, 10, 4};

    std::vector<float> robot_theta_limit(2);
    robot_theta_limit = {-0.1, 0.1};

    // angle cahnge per time:
    float angle_change = (robot_theta_limit[1] - robot_theta_limit[0]) / robot_angle_number;
    clear_pedsim_agent.call(place_holder_msg);

    // success flag
    bool del_result;
    bool add_result;

    image_transport::ImageTransport it_(nh);
    image_transport::Subscriber image_sub_;
    int count = 0;
    int& counter = count;
    while(ros::ok()){

        clear_pedsim_agent.call(place_holder_msg);
        ros::Duration(2).sleep();
        for(int i=0; i<current_model_number-num_static_obstacle; i++){
            // ros::Duration(0.1).sleep();
            ROS_INFO("Trying to delete buiding models");
            model_delete_msg.request.model_name = "building_" + std::to_string(i);
            delete_model_client.call(model_delete_msg);
            del_result = model_delete_msg.response.success;
            if(!del_result){
                ROS_WARN("delete error occurs once");
            }
        }
        // for(int i=0; i<current_model_number - num_static; i++){
        for(std::string name : curr_obstacle_arr){
            // ros::Duration(0.1).sleep();
            ROS_INFO("Trying to delete obstacle models");
            model_delete_msg.request.model_name = name;
            delete_model_client.call(model_delete_msg);
            del_result = model_delete_msg.response.success;
            if(!del_result){
                ROS_WARN("delete error occurs once");
            }
        }




        delete_agent_models(model_delete_msg,delete_model_client,counter);
        counter +=1;

        add_static_obstacles(num_static_obstacle,static_obstacle_xml_lib,static_obstacle_ori_name_lib,spawn_model_client);
        
        add_static_building(num_static, static_xml_lib, static_ori_name_lib, spawn_model_client);

        // current_model_number = std::rand() % max_num_mobile + 1 + num_static;
        current_model_number = num_static + num_static_obstacle;

        // std::vector<std::vector<float>> store_values;
        restart_pedsim_agent.call(place_holder_msg);
        // ros::Duration(1).sleep();


        // current_model_number = 1;
        // for(int i=num_static; i<current_model_number; i++){
        //     int temp_idx = std::rand() % mobile_xml_lib.size();
        //     float theta = float(std::rand() % 100)/99.0 * PI;
        //     get_add_message(model_add_msg, i, mobile_xml_lib[temp_idx], mobile_ori_name_lib[temp_idx], location_limit_p, theta);
        //     // cout << model_add_msg.request.model_xml;
        //     spawn_model_client.call(model_add_msg);

        //     std::vector<float> current_model_data(8);
            
        //     current_model_data[0] =  temp_idx;
        //     current_model_data[1] =  model_add_msg.request.initial_pose.position.x;
        //     current_model_data[2] =  model_add_msg.request.initial_pose.position.y;
        //     current_model_data[3] =  model_add_msg.request.initial_pose.position.z;
        //     current_model_data[4] =  theta;
        //     current_model_data[5] =  mobile_scale[temp_idx][0];
        //     current_model_data[6] =  mobile_scale[temp_idx][1];
        //     current_model_data[7] =  mobile_scale[temp_idx][2];

        //     store_values.push_back(current_model_data);

        //     //make sure service call was successful
        //     add_result = model_add_msg.response.success;
        //     if (!add_result){
        //         ROS_WARN("generate error occurs once");
        //     }
        // }

        // ros::spinOnce();
        image_sub_ = it_.subscribe("/rrbot/camera1/image_rect_color", 1, imageCb);

        
             
        past_model_number = current_model_number;
        for(int i = 0; i<robot_state_number; i++){
            float temp_x = float(std::rand() % 101) / 100.0 * (location_limit_robot[2] - location_limit_robot[0]) + location_limit_robot[0];
            float temp_y = float(std::rand() % 101) / 100.0 * (location_limit_robot[3] - location_limit_robot[1]) + location_limit_robot[1];
            for(int j = 0; j<robot_angle_number; j++){
                float temp_theta = robot_theta_limit[0] + (j+1) * (float(std::rand() % 101) / 100.0) * angle_change;
                get_change_msg(model_state_msg, temp_x, temp_y, temp_theta);
                set_model_state_client.call(model_state_msg);
                std::vector<float> camera_status(4);
                camera_status[0] = temp_x;
                camera_status[1] = temp_y;
                camera_status[2] = 0;
                camera_status[3] = temp_theta;
                // int len = store_values.size();
                // if (store_values[len-1].size() == 8){
                //     store_values.push_back(camera_status);
                // }
                // else{
                //     store_values[len-1] = camera_status;
                // }
                // std::ofstream output_file("src/generate_data/data/label/" + std::to_string(IMG_ID) + ".txt");
                // std::ostream_iterator<float> output_iterator(output_file, " ");
                // for (auto item : store_values){
                //     std::copy(item.begin(), item.end(), output_iterator);
                //     output_file << '\n';
                // }
                // output_file.close();
                ros::Duration(20).sleep();
                IMG_ID += 1;
                
            }

        }
        ros::spinOnce();

        


        //TODO: save_image

        // save gt
    }

       
}