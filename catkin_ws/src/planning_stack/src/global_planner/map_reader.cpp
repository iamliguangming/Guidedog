// This node reads in the occupancy grid msgs from map service
// Author: Yongxin Guo
// Contact: yongxin@seas.upenn.edu
#include "map_reader.h"
#include <iostream>
#include <tf2_ros/static_transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <tf2/LinearMath/Quaternion.h>
#include <vector>

MapReader::MapReader(ros::NodeHandle* nh){
    request_map(nh); // establish the client-server relationship
    get_map_data(); // retrieve the map data for occupancy grid
    publish_transform(); // publish transform
}

MapReader::~MapReader(){
}

void MapReader::publish_transform(){
    // start transformation between map and world frame
    static tf2_ros::StaticTransformBroadcaster static_broadcaster;
    geometry_msgs::TransformStamped static_transformStamped;

    static_transformStamped.header.stamp = ros::Time::now();
    static_transformStamped.header.frame_id = map_frame;
    static_transformStamped.child_frame_id = world_frame;
    
    // assign the offset value
    tf2::Quaternion quat;
    // quat.setRPY(0, 0, -1.57);
    quat.setRPY(0, 0, 0);
    static_transformStamped.transform.translation.x = -(height * resolution) / 2;
    static_transformStamped.transform.translation.y = -(width * resolution) / 2;
    static_transformStamped.transform.translation.z = 0;
    static_transformStamped.transform.rotation.x = quat.x();
    static_transformStamped.transform.rotation.y = quat.y();
    static_transformStamped.transform.rotation.z = quat.z();
    static_transformStamped.transform.rotation.w = quat.w();
    static_broadcaster.sendTransform(static_transformStamped);
}

void MapReader::request_map(ros::NodeHandle* nh){
     while (!ros::service::waitForService(srv_name, ros::Duration(3.0))) {
            ROS_INFO("Waiting for the %s service to be available", srv_name.c_str());
    }
    ROS_INFO("Requesting the map...");
    mapClient = nh->serviceClient<nav_msgs::GetMap>("static_map");
}

void MapReader::get_map_data(){
    if (mapClient.call(mapSrv)){
        map = mapSrv.response.map;            
    }
    height = map.info.height; // row
    width = map.info.width;  // col
    resolution = map.info.resolution;
    map_frame = map.header.frame_id;
    resolution_int = int(resolution_factor * resolution);
    ROS_INFO("Map Received. Size: %d X %d with %.3f m/cell",
            width, height, resolution);
            
    // Initialize the size of 2-D grid. Height -> rows. Width -> cols.
    occupancyGrid.resize(height);

    for (int row = 0; row < height; row++) {
        occupancyGrid[row].resize(width);
    }

    int cellCounter = 0;
    for (int row = 0; row < height; row++){
        for (int col = 0; col < width; col++){
            if (map.data.at(cellCounter) == -1){
                // Not occupied
                occupancyGrid[row][col] = false;
            }
            else{
                // Occupied
                occupancyGrid[row][col] = true;
            }
            // update total cell counter
            cellCounter ++;
        }
    }

    // inflate the map according to inflation layer setting for avoiding the robot going along the wall
    not_inflated_occupancyGrid = occupancyGrid;
    inflate_map(occupancyGrid);
}

void MapReader::inflate_map(std::vector<std::vector<bool>> &occupancyGrid){
    // inflate every vertical wall by reading the map from horizontal direction(left to right)
    for(int i = 0; i < height; i++){
        int layer_counter = 0;
        for(int j = 0; j < width - 1; j++){
            if((occupancyGrid[i][j] != occupancyGrid[i][j+1]) && (occupancyGrid[i][j] == true)){
                occupancyGrid[i][j+1] = true;
                layer_counter++;
                if(layer_counter == inflation_layer){
                    j++;  // if layer counter reach the preset layer thickness, skip the next cell for stopping the inflation process
                    layer_counter = 0;  // set layer counter to zero.
                }
            }
        }

        // reverse the horizontal inflation process to complete all the walls.
        layer_counter = 0; // reinitialize layer counter for reverse
        for(int j = width - 1; j > 1; j--){
            if((occupancyGrid[i][j] != occupancyGrid[i][j-1]) && (occupancyGrid[i][j] == true)){
                occupancyGrid[i][j-1] = true;
                layer_counter++;
                if(layer_counter == inflation_layer){
                    j--;
                    layer_counter = 0;
                }
            }
        }
    }

    // inflate every horizontal wall by reading the map from vertical direction(up to down)
    for(int j = 0; j < width; j++){
        int layer_counter = 0;
        for(int i = 0; i < height - 1; i++){
            if((occupancyGrid[i][j] != occupancyGrid[i + 1][j]) && (occupancyGrid[i][j] == true)){
                occupancyGrid[i + 1][j] = true;
                layer_counter++;
                if(layer_counter == inflation_layer){
                    i++;
                    layer_counter = 0;
                }
            }
        }

        // reverse direction
        layer_counter = 0;
        for(int i = height - 1; i > 1; i--){
            if((occupancyGrid[i][j] != occupancyGrid[i-1][j]) && (occupancyGrid[i][j] == true)){
                occupancyGrid[i-1][j] = true;
                layer_counter++;
                if(layer_counter == inflation_layer){
                    i--;
                    layer_counter = 0;
                }
            }
        }
    }
}

int MapReader::getHeight(){
    return height;
}

int MapReader::getWidth(){
    return width;
}

long int MapReader::get_map_size(){
    long int map_size = height * width;
    return map_size;
}


double MapReader::getResolution(){
    return resolution;
}

std::string MapReader::get_world_frame_id(){
    return world_frame;
}

bool MapReader::get_occ_val(const std::vector<double> &xy_double){
    // int i = int(x * resolution_factor) / resolution_int;
    // int j = int(y * resolution_factor) / resolution_int;
    curr_grid_pos = MapReader::get_curr_grid_pos(xy_double);
    int i = curr_grid_pos[0];
    int j = curr_grid_pos[1];

    return occupancyGrid[i][j];
}

std::vector<std::vector<bool>> MapReader::get_local_occ_grid(const std::vector<double> &xy_double, const double &r){
    // int i = int(x * resolution_factor) / resolution_int;
    // int j = int(y * resolution_factor) / resolution_int;
    // ROS_INFO("map size: %d %d", width, height);
    // ROS_INFO("map resolution: %f", resolution);
    // ROS_INFO("curr x double: %f", xy_double[0]);
    // ROS_INFO("curr y double: %f", xy_double[1]);
    std::vector<int> curr_grid_pos = get_curr_grid_pos(xy_double);
    // ROS_INFO("curr grid pos i:%d", curr_grid_pos[0]);
    // ROS_INFO("curr grid pos j:%d", curr_grid_pos[1]);
    int i = curr_grid_pos[0];
    int j = curr_grid_pos[1];
    int i_start = i - int( (r - resolution / 2) * resolution_factor) / resolution_int - 1; // floor the cell
    int i_end = i + (i - i_start);
    int j_start = j - int( (r - resolution / 2) * resolution_factor) / resolution_int - 1; // floor the cell
    int j_end = j + (j - j_start);

    // Initialize the size of 2-D local grid. Height -> rows. Width -> cols.
    local_occupancyGrid.resize(i_end - i_start + 1);

    for(int row = i_start; row <= i_end; row++){
        local_occupancyGrid[row - i_start].resize(j_end - j_start + 1);
    }
    // Assign the local grid map
    for(int row = i_start; row <= i_end; row++){
        for(int col = j_start; col <= j_end; col++){
            local_occupancyGrid[row - i_start][col - j_start] = not_inflated_occupancyGrid[row][col];
        }
    }
    
    return local_occupancyGrid;
}

// Transfer x-y double type to x-y int. discretized type
std::vector<int> MapReader::get_curr_grid_pos(const std::vector<double> &xy_double){
    double x = xy_double[0];
    double y = xy_double[1];    
    curr_grid_pos[0] = int(y * resolution_factor) / resolution_int;
    curr_grid_pos[0] = (height - 1) - curr_grid_pos[0]; // flip y axis. 
    curr_grid_pos[1] = int(x * resolution_factor) / resolution_int;
    return curr_grid_pos;
}

// Trnsfer x-y int. discretized type to x-y double type
std::vector<double> MapReader::get_xy_double_from_coord(const std::vector<int> &curr_grid_coord){
    std::vector<double> xy_double = {(height - curr_grid_coord[0]) * resolution + resolution / 2.0, 
                                curr_grid_coord[1] * resolution + resolution / 2.0};
    return xy_double;
}

// Transfer x-y double type to one-dimensional index
int MapReader::get_curr_pos_idx(const std::vector<double> &xy_double){
    curr_grid_pos = MapReader::get_curr_grid_pos(xy_double);
    // Transfer it into one dimension index. Easier for implementation in Dijkstra.
    curr_pos_idx = curr_grid_pos[0] * width + curr_grid_pos[1];
    return curr_pos_idx;
}

// Transfer one-dimensional index to x-y double type
std::vector<double> MapReader::get_xy_double_from_idx(const int &curr_pos_idx){
    std::vector<int> temp = MapReader::get_coord_from_idx(curr_pos_idx);
    return MapReader::get_xy_double_from_coord(temp);      
}

// Transfer one dimensional index to x-y int. discretized type
std::vector<int> MapReader::get_coord_from_idx(const int &cell_idx){
    std::vector<int> coord;
    coord.resize(2);
    coord[0] = cell_idx / width;
    coord[1] = cell_idx % width;
    
    return coord;
}

// Transfer x-y int. discretized type to one dimensional index.
int MapReader::get_idx_from_coord(const std::vector<int> &cell_coord){
    int cell_idx = cell_coord[0] * width + cell_coord[1];

    return cell_idx;
}

bool MapReader::is_colliding(const std::vector<int> &cell_coord, const bool &is_map_inflated){
    if(is_map_inflated){
        return occupancyGrid[cell_coord[0]][cell_coord[1]];
    }
    else{
        return not_inflated_occupancyGrid[cell_coord[0]][cell_coord[1]];
    }
}

bool MapReader::is_outside_map(const std::vector<int> &cell_coord){
    if(cell_coord[0] < 0 || cell_coord[0] >= height || cell_coord[1] < 0 || cell_coord[1] >= width ){
        return true;
    }
    else{
        return false;
    }
}
