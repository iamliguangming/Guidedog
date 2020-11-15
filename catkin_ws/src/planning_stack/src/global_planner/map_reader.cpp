// This node reads in the occupancy grid msgs from map service
// Author: Yongxin Guo
// Contact: yongxin@seas.upenn.edu
#include "map_reader.h"
#include <iostream>

MapReader::MapReader(ros::NodeHandle* nh){
    request_map(nh); // establish the client-server relationship
    get_map_data(); // retrieve the map data for occupancy grid
}

MapReader::~MapReader(){
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

bool MapReader::get_occ_val(const double &x, const double &y){
    // int i = int(x * resolution_factor) / resolution_int;
    // int j = int(y * resolution_factor) / resolution_int;
    int* curr_grid_pos = MapReader::get_curr_grid_pos(x ,y);
    int i = *curr_grid_pos;
    int j = *(curr_grid_pos + 1);

    return occupancyGrid[i][j];
}

std::vector<std::vector<bool>> MapReader::get_local_occ_grid(const double &x, const double &y, const double &r){
    // int i = int(x * resolution_factor) / resolution_int;
    // int j = int(y * resolution_factor) / resolution_int;
    int* curr_grid_pos = MapReader::get_curr_grid_pos(x ,y);
    int i = *curr_grid_pos;
    int j = *(curr_grid_pos + 1);
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
    for(int row= i_start; row <= i_end; row++){
        for(int col = j_start; col <= j_end; col++){
            local_occupancyGrid[row - i_start][col - j_start] = occupancyGrid[row][col];
        }
    }
    
    return local_occupancyGrid;
}

int* MapReader::get_curr_grid_pos(const double &x, const double &y){
    curr_grid_pos[0] = int(y * resolution_factor) / resolution_int;
    curr_grid_pos[0] = (height - 1) - curr_grid_pos[0]; // flip y axis.  
    curr_grid_pos[1] = int(x * resolution_factor) / resolution_int;
    return curr_grid_pos;
}

int MapReader::get_curr_pos_idx(const double &x, const double &y){
    int* curr_grid_pos = MapReader::get_curr_grid_pos(x, y);
    // Transfer it into one dimension index. Easier for implementation in Dijkstra.
    curr_pos_idx = *curr_grid_pos * width + *(curr_grid_pos + 1);
    return curr_pos_idx;
}

std::vector<int> MapReader::get_coord_from_idx(const int &cell_idx){
    std::vector<int> coord;
    coord.resize(2);
    coord[0] = cell_idx / width;
    coord[1] = cell_idx % width;
    
    return coord;
}

int MapReader::get_idx_from_coord(const std::vector<int> &cell_coord){
    int cell_idx = cell_coord[0] * width + cell_coord[1];

    return cell_idx;
}

bool MapReader::is_colliding(const std::vector<int> &cell_coord){
    return occupancyGrid[cell_coord[0]][cell_coord[1]];
}

bool MapReader::is_outside_map(const std::vector<int> &cell_coord){
    if(cell_coord[0] < 0 || cell_coord[0] >= height || cell_coord[1] < 0 || cell_coord[1] >= width ){
        return true;
    }
    else{
        return false;
    }
}