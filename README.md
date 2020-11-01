## For Developers

  - .world files should be put inside Guidedog/catkin_ws/src/turtlebot3_simulations/turtlebot3_gazebo/worlds
  - .world file related meshes should be put inside Guidedog/catkin_ws/src/turtlebot3_simulations/turtlebot3_gazebo/models, make sure to include all your other resources here as well!!!
  - Duplicate Guiding_Crossing.launch from Guidedog/catkin_ws/src/turtlebot3_simulations/turtlebot3_gazebo/launch and change line 12-13 from 
  ```
    <arg name="scene_file" default="social_contexts"/>
    <arg name="world_file" default="streettest"/>
  ```
  to
  ```
    <arg name="scene_file" default="name_of_your_scene"/>
    <arg name="world_file" default="name_of_your_world"/>
  ```
  now rerun ```catkin_make``` in the root directory
  run
  ```
  roslaunch turtlebot3_gazebo <name_of_your_duplicated_launch_file>.launch 
  ```  
  The launch file will automatically launch turtlebot3_teleop_key.node and enables teleoperation using keyboards  
  You can disable line 63-65 it if you don't need this functionality in your simulations.  
  
### RVIZ Visualization
  If you want to run RVIZ to visualize the laser scan received from the turtlebot3 alone with the view from onboard  
  camera, you can run the following lines in terminal to boot up the RVIZ application  
  ```  
  roslaunch turtlebot3_gazebo turtlebot3_gazebo_rviz.launch  
  ```  
### Pedsim Plugin
  For using .world file along with pedsim agents, it is required to add the following lines at the end of .world file  
  before tag </world> and </sdf> in order to enable the pedsim agent plugin.   
  ```  
  <plugin name="ActorPosesPlugin" filename="libActorPosesPlugin.so">
  </plugin>  
  ```
### 2D Map
  In order to convert the 3D gazebo world to 2D occupancy grid map, please add the following line in between the <world> </world> tags of your Gazebo world file(it is recommended to add it to the end of world file):  
```  
<plugin name='gazebo_occupancy_map' filename='libgazebo_2Dmap_plugin.so'>
    <map_resolution>0.1</map_resolution> <!-- in meters, optional, default 0.1 -->
    <map_height>0.3</map_height>         <!-- in meters, optional, default 0.3 -->
    <map_size_x>10</map_size_x>          <!-- in meters, optional, default 10 -->
    <map_size_y>10</map_size_y>          <!-- in meters, optional, default 10 -->
    <init_robot_x>0</init_robot_x>          <!-- x coordinate in meters, optional, default 0 -->
    <init_robot_y>0</init_robot_y>          <!-- y coordinate in meters, optional, default 0 -->
</plugin>  
```  
You can adjust the parameters according to your need inside the plugin tab.

### Generate the 2D map  
Please run the following command to generate the map given a world file:
```
roslaunch planning_stack 2D_map_generator.launch  
```  
You can replace the world file to the one you need inside the 2D_map_generator.launch file by changing the line  
```
<arg name="world_file" default="newstreet_4blocks"/>
```
to  
```
<arg name="world_file" default="<your_world_file_name>"/>
```
After you run the launch file, you can call the /gazebo_2Dmap_plugin/generate_map ros service  
to generate the map by publishing the map information to /map2d ros topic:  
```  
rosservice call /gazebo_2Dmap_plugin/generate_map  
```  
You can use the map_saver node from the map_server package inside ros navigation to save your  
generated map to a .pgm and .yaml file:  
```
rosrun map_server map_saver -f <mapname> /map:=/map2d  
```
You can visualize the occupancy map by adding /map2d topic to your RVIZ window.  
In the future development case that you've already have a map file and you want  
to read from it and publish to a topic or create a service, you can run  
```
rosrun map_server map_server <name_of_your_map_file>.yaml  
```
It will publish the map to /map topic and create a service /static_map.

## Installation

### Basic Installation
  Some basic guidelines on the installation
```
git clone https://github.com/iamliguangming/Guidedog
cd Guidedog
git submodule update --init --recursive
cd catkin_ws
catkin_make
source devel/setup.bash
```  
There are three main turtlebot models: waffle, waffle_pi, burger. You also have to setup the environment  
variables in order to set a specific model. Type the following in the command line for opening up .bashrc  
file
```  
sudo nano ~/.bashrc  
```  
Then add the following line at the end of .bashrc file to setup the environment variable  
```
export TURTLEBOT3_MODEL=burger  
```  
Then source the .bashrc file to refresh the current shell  
```
source ~/.bashrc  
```  
Please be noted it is required to source devel/setup.bash file everytime when you open up a new shell tab  
unless it is added to the ~/.bashrc file. If not doing so, it will result that the rospack not being able  
to locate your ROS packages. It usually occurs when you are using roslaunch command in the terminal.

### Gazebo_ros_2Dmap_plugin  
  In order to use the plugin for generating 2D map given a 3D gazebo world. It is required to  
install octomap, costmap_2d, and ROS map server packages using the following command:  
```  
sudo apt-get install ros-melodic-octomap  
sudo apt-get install ros-melodic-costmap-2d  
sudo apt-get install ros-melodic-map-server  
```  


## Implementation  

  For the use of customize package for the future development instead of running demo launch from pedsim  
submodules as illustrated above, please use the following command line for the customize world simulation  
in future project implementation  
  In details, the command line examples in **installation** section provided by the pedsim officials, it runs  
scenarios or so-called activities alone from launch file under 'pedsim_simulator' pkg and aslo boots up RVIZ  
for monitoring. Then it boots up the gazebo world and simultaneously take in the agent activities. It is not  
needed anymore inside the customized world_simulations pkg. one-liner in terminal is used as follows: 
```
roslaunch world_simulations <customized_launch_file>.launch
```  
  In this case, we have provided some launch files inside 'launch' folder under 'world_simulations' pkg.  
You can run **my_sim.launch** as the instance. In future, it will be replaced as customized launch files.  
It will be required to put self-defined .world gazebo models and .xml scenario file inside the 'worlds'  
and 'scenarios' folder under 'world_simulations' pkg. Then a quick shorcut is to directly modify  
'my_sim.launch' file to reflect the changes in world model and scenario files. The launch file will  
automatically invoke the pedsim submodules simulators and associated tools to simulate the target world  
and the scenario defined by users.  

## What is included:
- A Pedstrain simulator from https://github.com/srl-freiburg/pedsim_ros
- A turtlebot3 simulator
- Gazebo-ros-2Dmap-plugin from https://github.com/marinaKollmitz/gazebo_ros_2Dmap_plugin/tree/melodic-devel

## Installed Pakcages:
- Pedestrian Simulator (ped_sim)
- Turtlebot3
- Gazebo_ros_2Dmap_plugin
## Branch Info:
- This is the branch for World and Robot Model/Sensor Development
