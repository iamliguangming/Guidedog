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
  
  If you want to run RVIZ to visualize the laser scan received from the turtlebot3 alone with the view from onboard  
  camera, you can run the following lines in terminal to boot up the RVIZ application  
  ```  
  roslaunch turtlebot3_gazebo turtlebot3_gazebo_rviz.launch  
  ```  
  
  For using .world file along with pedsim agents, it is required to add the following lines at the end of .world file  
  before tag </world> and </sdf> in order to enable the pedsim agent plugin.   
  ```  
  <plugin name="ActorPosesPlugin" filename="libActorPosesPlugin.so">
  </plugin>  
  ```
  If using streettest.world. It is required to change some lines to reflect your local directory, please change the  
  line **108 and 122** in streettest.world file and line **23 and 45** in /simplemodified/model.sdf file under the  
  worlds and models folder. It is recommended to comment out the previous users'directories.  
  
## How to geneate bagfiles for data collection
Assume you have already done:
```
source devel/setup.bash
```
First run:
```
roslaunch turtlebot3_gazebo turtlebot3_generate_data_world.launch 
```
Then open another terminal and run:
```
rosrun set_state gazebo_set_state 

```
Then open another terminal and:
```
mkdir rosbag
cd rosbag
rosbag record -O <name_of_your_bag_file> /rrbot/camera1/camera_info /rrbot/camera1/image_raw /gazebo/model_states /clock /tf
```
After you're done with recording, press ctrl+c to end the process.
The bagfile is large in volume, a 240s bagfile would occupy around 16GB of storage. Changing the samping rate might be a good idea to reduce the volume.
## Installation

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

## Installed Pakcages:
- Pedestrian Simulator (ped_sim)
- Turtlebot3
## Branch Info:
- This is the branch for World and Robot Model/Sensor Development
