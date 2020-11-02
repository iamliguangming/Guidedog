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

#define PI 3.1415926

static int IMG_ID = 0;

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
        std::string model_name = "model_"  + std::to_string(i);
        static_add_msg.request.model_name = model_name;
        std::string xml = static_xml_lib[temp_idx];
        std::string o_name = static_ori_name_lib[temp_idx];
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

    std::vector<std::string> input_mobile_xml;
    std::vector<std::string> input_mobile_name;
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


    modle_define(input_mobile_xml, input_mobile_name, mobile_xml_lib, mobile_ori_name_lib);
    modle_define(input_static_xml, input_static_name, static_xml_lib, static_ori_name_lib);
       
    ros::Rate loop_rate(10);

    // generate parameters
    int num_static = 6;
    int max_num_mobile = 10;
    int robot_state_number = 5;
    int robot_angle_number = 3;


    int past_model_number = 0;
    int current_model_number = 0;
    
    
    // location limit
    std::vector<float> location_limit_p(4);
    location_limit_p = {2, 1, 10, 5};

    std::vector<float> location_limit_robot(4);
    location_limit_robot = {-2, 2, 1, 4};

    std::vector<float> robot_theta_limit(2);
    robot_theta_limit = {-0.2, 0.2};

    // angle cahnge per time:
    float angle_change = (robot_theta_limit[1] - robot_theta_limit[0]) / robot_angle_number;


    // success flag
    bool del_result;
    bool add_result;

    while(ros::ok()){
        for(int i=0; i<past_model_number; i++){
            model_delete_msg.request.model_name = "model_" + std::to_string(i);
            delete_model_client.call(model_delete_msg);
            del_result = model_add_msg.response.success;
            if(!del_result){
                ROS_WARN("delete error occurs once");
            }
        }
        
        add_static_building(num_static, static_xml_lib, static_ori_name_lib, spawn_model_client);

        current_model_number = std::rand() % max_num_mobile + 1 + num_static;

        std::vector<std::vector<float>> store_values;

        // current_model_number = 1;
        for(int i=num_static; i<current_model_number; i++){
            int temp_idx = std::rand() % mobile_xml_lib.size();
            float theta = float(std::rand() % 100)/99.0 * PI;
            get_add_message(model_add_msg, i, mobile_xml_lib[temp_idx], mobile_ori_name_lib[temp_idx], location_limit_p, theta);
            // cout << model_add_msg.request.model_xml;
            spawn_model_client.call(model_add_msg);

            std::vector<float> current_model_data(8);
            
            current_model_data[0] =  temp_idx;
            current_model_data[1] =  model_add_msg.request.initial_pose.position.x;
            current_model_data[2] =  model_add_msg.request.initial_pose.position.y;
            current_model_data[3] =  model_add_msg.request.initial_pose.position.z;
            current_model_data[4] =  theta;
            current_model_data[5] =  mobile_scale[temp_idx][0];
            current_model_data[6] =  mobile_scale[temp_idx][1];
            current_model_data[7] =  mobile_scale[temp_idx][2];

            store_values.push_back(current_model_data);

            //make sure service call was successful
            add_result = model_add_msg.response.success;
            if (!add_result){
                ROS_WARN("generate error occurs once");
            }
        }
        
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
                int len = store_values.size();
                if (store_values[len-1].size() == 8){
                    store_values.push_back(camera_status);
                }
                else{
                    store_values[len-1] = camera_status;
                }
                std::ofstream output_file("src/generate_data/data/label/" + std::to_string(IMG_ID) + ".txt");
                std::ostream_iterator<float> output_iterator(output_file, " ");
                for (auto item : store_values){
                    std::copy(item.begin(), item.end(), output_iterator);
                    output_file << '\n';
                }
                output_file.close();
                IMG_ID += 1;
                ros::Duration(1).sleep();
            }
        }

        


        //TODO: save_image

        // save gt
    }

       
}
