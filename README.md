## Installation

Some basic guidelines on the installation
```
git clone https://github.com/iamliguangming/Guidedog 
cd Guidedog
git submodule update --init --recursive
source catkin_ws/devel/setup.bash
roslaunch pedsim_simulator airport_demo.launch 
roslaunch pedsim_gazebo_plugin airport.launch
```
After cloning the repo, you are required to delete the 'devel' and 'build' directory inside the catkin_ws in order to use catkin_make in your own device. 
'devel' and 'build' file will be created again after you run catkin_make.  

## Implementation  

For the use of customize package for the future development instead of running demo launch from pedsim submodules as illustrated above,  
please use the following command line for the customize world simulation in future project implementation 
```
roslaunch world_simulations <customized_launch_file>.launch
```  
In this case, we have provided some launch files inside 'launch' folder under 'world_simulations' pkg.  
You can run airport.launch as the instance. In future, it will be replaced as customized launch files.  
It will be required to put self-defined .world gazebo models and .xml scenario file inside the 'worlds'  
and 'scenarios' folder under 'world_simulations' pkg. Then a quick shorcut is to directly modify  
'airport.launch' file to reflect the changes in world model and scenario files. The launch file will  
automatically invoke the pedsim submodules simulators and associated tools to simulate the target world  
and the scenario defined by users. 
## What is included:
- A Pedstrain simulator from https://github.com/srl-freiburg/pedsim_ros
- Nothing more for now

## Installed Pakcages:
- Pedestrian Simulator (ped_sim)
- Turtlebot3
## Branch Info:
- This is the branch for World and Robot Model/Sensor Development
