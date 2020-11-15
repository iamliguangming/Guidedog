// A node file consisting of implementation of Dijkstra algorithm for global planning
#include "map_reader.h"
#include "global_path_finder.h"
#include <ros/ros.h>

#define INF 0x3f3f3f3f

GlobalPathFinder::GlobalPathFinder(const std::string& method, MapReader* input_map){
    this->method = method;
    this->map = input_map;
    // Initialize the cost vector
    const long int map_size = map->get_map_size();
    cost.resize(map_size);
    for(int i = 0; i < map_size; i++){
        cost[i] = INF;
    }
}


void GlobalPathFinder::add_adjacent_cell(const int& cell_idx){
    // Loop through all the neighbors. 8-connected graph
    std::vector<int> cell_coord = map->get_coord_from_idx(cell_idx);
    int row_shift[8] = {0, -1, -1, -1, 0, 1, 1, 1};
    int col_shift[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int dist[8] = {5, 7, 5, 7, 5, 7, 5, 7};

    for(int i = 0; i < 8; i++){
        std::vector<int> neighbor_coord = {cell_coord[0] + row_shift[i], cell_coord[1] + col_shift[i]};
        if (!map->is_outside_map(neighbor_coord) && !map->is_colliding(neighbor_coord)){
            int curr_idx = map->get_idx_from_coord(neighbor_coord);
            curr_adjacent_list.push_back(std::make_pair(dist[i],curr_idx));
        }
    }
}

std::vector<std::vector<int>> GlobalPathFinder::find_path(const std::vector<int> start_cell_coord, const std::vector<int> goal_cell_coord){
    // Check if the start cell is inside the obstacle
    if(map->is_colliding(start_cell_coord) || map->is_colliding(goal_cell_coord)){
        ROS_WARN("The start location is inside the obstacle! Please evaluate your start point.");
        return path;
    }
    // push back the start point into heap and cost of it into cost vector
    int start_cell = map->get_idx_from_coord(start_cell_coord);
    int goal_cell = map->get_idx_from_coord(goal_cell_coord);

    cost[start_cell] = 0;
    cost_min_heap.insert(std::make_pair(0, start_cell));
    
    // Dijkstra algorithm
    while (!cost_min_heap.empty()){
        std::pair<int, int> curr_cell = *(cost_min_heap.begin()); // extract the cell with minimum cost
        cost_min_heap.erase(cost_min_heap.begin()); // remove it from the unvisited sets

        int curr_cell_idx = curr_cell.second;
        if(curr_cell_idx == goal_cell) break;  // goal reached. Stop dijkstra.

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
                    }
                }
                cost[adj_idx] = cost[curr_cell_idx] + dist_to_adj; // update the cell's cost
                cost_min_heap.insert(std::make_pair(cost[adj_idx], adj_idx));
                parents.push_back(std::make_pair(curr_cell_idx, adj_idx));
            }
        }
        curr_adjacent_list.clear(); // remove all the neighbor cell for next time use.
    }

    std::list<std::pair<int, int>>::iterator it_parents;

    // Extract the path
    int backtrace_cell = goal_cell;
    while(true){
        for(it_parents = parents.begin(); it_parents != parents.end(); it_parents++){
            if((*it_parents).second == backtrace_cell){
                std::vector<int> path_coord = map->get_coord_from_idx((*it_parents).second);
                path.push_back(path_coord); // store the child cell
                backtrace_cell = (*it_parents).first; // get the parent cell
                break;
            }
        }
        if(backtrace_cell == start_cell){
            // remove the goal cell. we also don't need the start cell in the path. Only the middle ones are needed.
            path.erase(path.begin());
            break; 
        }
    }
    
    return path;
}

