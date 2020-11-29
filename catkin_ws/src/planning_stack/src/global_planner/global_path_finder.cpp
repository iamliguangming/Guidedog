// A node file consisting of implementation of Dijkstra algorithm for global planning
#include "map_reader.h"
#include "global_path_finder.h"
#include <ros/ros.h>
#include <nav_msgs/Path.h>
#include <geometry_msgs/PoseStamped.h>
#include <algorithm>
#include <math.h>


#define INF 0x3f3f3f3f

GlobalPathFinder::GlobalPathFinder(ros::NodeHandle *nh, const std::string& method, MapReader* input_map){
    this->method = method;
    this->map = input_map;
    this->nh = nh;
    path_pub = nh->advertise<nav_msgs::Path>(path_topic, 1);
    nav_goal_sub = nh->subscribe(nav_goal_topic, 10, &GlobalPathFinder::receive_goal, this);
    odom_sub = nh->subscribe("/odom",1, &GlobalPathFinder::Odom_call_back,this);    // robot global pose
    // Initialize the cost vector
    const long int map_size = map->get_map_size();
    cost.resize(map_size);
    for(int i = 0; i < map_size; i++){
        cost[i] = INF;
    }
    // Initialize the start position
    initial_pose = {165.45, 63.75};
}


void GlobalPathFinder::add_adjacent_cell(const int& cell_idx){
    // Loop through all the neighbors. 8-connected graph
    std::vector<int> cell_coord = map->get_coord_from_idx(cell_idx);
    int row_shift[8] = {0, -1, -1, -1, 0, 1, 1, 1};
    int col_shift[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int dist[8] = {5, 7, 5, 7, 5, 7, 5, 7};
    bool is_map_inflated = true;  // use inflation map for planning
    for(int i = 0; i < 8; i++){
        std::vector<int> neighbor_coord = {cell_coord[0] + row_shift[i], cell_coord[1] + col_shift[i]};
        if (!map->is_outside_map(neighbor_coord) && !map->is_colliding(neighbor_coord, is_map_inflated)){
            int curr_idx = map->get_idx_from_coord(neighbor_coord);
            curr_adjacent_list.push_back(std::make_pair(dist[i],curr_idx));
        }
    }
}



void GlobalPathFinder::find_path(const std::vector<double> &start_cell, const std::vector<double> &goal_cell){
    // Transfer double type to discretized integer type
    std::vector<int> start_cell_coord = map->get_curr_grid_pos(start_cell);
    std::vector<int> goal_cell_coord = map->get_curr_grid_pos(goal_cell);

    // ROS_INFO("start cell coord: (%d, %d)", start_cell_coord[0], start_cell_coord[1]);
    // ROS_INFO("goal cell coord: (%d, %d)", goal_cell_coord[0], goal_cell_coord[1]);
<<<<<<< Updated upstream
    // Check if the start cell is inside the obstacle
=======
    // Check if the selected start and goal cell is inside the obstacle
>>>>>>> Stashed changes
    bool is_map_inflated = false;  // for selection, use non-inflated map.
    if(map->is_colliding(start_cell_coord, is_map_inflated) || map->is_colliding(goal_cell_coord, is_map_inflated)){
        ROS_WARN("The start or goal location is inside the obstacle! Please evaluate your choice.");
        std::abort();
    }

    // check if the selected cell and goal cell is inside the inflation layer
    is_map_inflated = true;
    if(map->is_colliding(start_cell_coord, is_map_inflated) || map->is_colliding(goal_cell_coord, is_map_inflated)){
        ROS_WARN("The start or goal location is inside the inflation layer! Please evaluate your choice.");
        std::abort();
        // return path;
    }

    // push back the start point into heap and cost of it into cost vector
    int start_cell_idx = map->get_idx_from_coord(start_cell_coord);
    int goal_cell_idx = map->get_idx_from_coord(goal_cell_coord);

    cost[start_cell_idx] = 0;
    cost_min_heap.insert(std::make_pair(0, start_cell_idx));

    // Dijkstra algorithm
    while (!cost_min_heap.empty()){
        ROS_INFO("Looking for Path ...");
        std::pair<int, int> curr_cell = *(cost_min_heap.begin()); // extract the cell with minimum cost
        cost_min_heap.erase(cost_min_heap.begin()); // remove it from the unvisited sets

        int curr_cell_idx = curr_cell.second;
        if(curr_cell_idx == goal_cell_idx) break;  // goal reached. Stop dijkstra.
        
        add_adjacent_cell(curr_cell_idx);

        std::list<std::pair<int, int>>::iterator it;
        std::list<std::pair<int, int>>::iterator it_parents;
        
        for(it = curr_adjacent_list.begin(); it != curr_adjacent_list.end(); it++){
            int dist_to_adj = (*it).first;
            int adj_idx = (*it).second;

            if (cost[adj_idx] > cost[curr_cell_idx] + dist_to_adj){
                if (cost[adj_idx] != INF){
                    // remove the neighbor cell from sets for updating
                    cost_min_heap.erase(cost_min_heap.find(std::make_pair(cost[adj_idx], adj_idx)));
                    // remove the parent-child relationship
                    for(it_parents = parents.begin(); it_parents != parents.end(); it_parents++){
                        if((*it_parents).second == adj_idx)
                            parents.erase(it_parents);
                            break;
                    }
                }
                cost[adj_idx] = cost[curr_cell_idx] + dist_to_adj; // update the cell's cost
                cost_min_heap.insert(std::make_pair(cost[adj_idx], adj_idx));
                parents.push_back(std::make_pair(curr_cell_idx, adj_idx));
            }
        }
        curr_adjacent_list.clear(); // remove all the neighbor cell for next time use.
    }

    ROS_INFO("Back sourcing ...");
    std::list<std::pair<int, int>>::iterator it_parents;
    // Extract the path
    int backtrace_cell_idx = goal_cell_idx;
    geometry_msgs::PoseStamped pose;
    // set use_sim_time false!!!! to publish.
    //pose.header.stamp = ros::Time::now();
    world_frame = map->get_world_frame_id();
    pose.header.frame_id = world_frame;
    path.header.frame_id = world_frame;
    while(true){
        for(it_parents = parents.begin(); it_parents != parents.end(); it_parents++){
            if((*it_parents).second == backtrace_cell_idx){
                //std::vector<int> path_coord = map->get_coord_from_idx((*it_parents).second);
                std::vector<double> path_coord = map->get_xy_double_from_idx((*it_parents).second);
                pose.pose.position.x = path_coord[1];
                pose.pose.position.y = path_coord[0];
                path.poses.push_back(pose);
                //path.push_back(path_coord); // store the child cell
                backtrace_cell_idx = (*it_parents).first; // get the parent cell
                break;
            }
        }
        if(backtrace_cell_idx == start_cell_idx){
            // remove the goal cell. we also don't need the start cell in the path. Only the middle ones are needed.
            path.poses.erase(path.poses.begin());
            break; 
        }
    }

    // reverse the path
    std::reverse(path.poses.begin(), path.poses.end());
    //return path;
}

void GlobalPathFinder::publish_path(){
    path_pub.publish(path);
}

void GlobalPathFinder::receive_goal(const geometry_msgs::PoseStamped &nav_goal_pose){
    double offset_x;
    double offset_y;
    if(nav_goal_pose.header.frame_id == "map"){  // if incoming poses are expressed in map frame, do the conversion
        offset_x = map_offset_x;
        offset_y = map_offset_y;
    }
    else if(nav_goal_pose.header.frame_id == "world"){  // if incoming poses are already expressed in world frame, conversion is not needed.
        offset_x = 0;
        offset_y = 0;
    }
    goal_received = {offset_x + nav_goal_pose.pose.position.x, offset_y + nav_goal_pose.pose.position.y};

    if(sqrt(pow((goal_received[0] - bot_pos.x), 2.0) + pow((goal_received[1] - bot_pos.y), 2.0)) >= 2.0){
        is_goal_received = true;
        // goal_received_pre = goal_received;
        ROS_INFO("Goal received!");
    }else{
        ROS_INFO("Goal too close to current position!");
    }
}

nav_msgs::Path GlobalPathFinder::fireup(){
    if(print_once_flag){
        ROS_INFO("Waiting for goal...");
        print_once_flag = false;
    }
    if(is_goal_received){
        path.poses.clear();        
        initial_pose[0] = bot_pos.x;
        initial_pose[1] = bot_pos.y;
        GlobalPathFinder::find_path(initial_pose, goal_received);
        is_goal_received = false;
        print_once_flag = true;
        // initial_pose = goal_received;
        ROS_INFO("Found Path: ");
        for(int i = 0; i < path.poses.size(); i++){
            ROS_INFO("Path Gazebo Coordinate: (%.2f, %.2f)", path.poses[i].pose.position.x - map_offset_x, path.poses[i].pose.position.y - map_offset_y);
        }
        cost_min_heap.clear();
        curr_adjacent_list.clear();
        parents.clear();
        const long int map_size = map->get_map_size();
        cost.resize(map_size);
        for(int i = 0; i < map_size; i++){
            cost[i] = INF;
        }
        return path;
    }
    nav_msgs::Path ignore;
    return ignore;
}

nav_msgs::Path GlobalPathFinder::get_path(){
    return path;
};

void GlobalPathFinder::Odom_call_back(const nav_msgs::Odometry::ConstPtr &msg){
    // ROS_INFO("in odom_call_back");
    bot_pos.x = msg->pose.pose.position.x + map_offset_x;
    bot_pos.y = msg->pose.pose.position.y + map_offset_y;
}


