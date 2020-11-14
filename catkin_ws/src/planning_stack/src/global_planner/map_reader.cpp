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
    height = map.info.height;
    width = map.info.width;
    resolution = map.info.resolution;
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

double MapReader::getResolution(){
    return resolution;
}

bool MapReader::get_occ_val(const int &i, const int &j){
    return occupancyGrid[i][j];
}

