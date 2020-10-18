## Installation

Some basic guidelines on the installation
```
git clone https://github.com/iamliguangming/Guidedog 
cd Guidedog
git submodule update --init --recursive
source catkin_ws/devel/setup.bash
roslaunch pedsim_simulator airport_demo.launch 
roslaunch pedsim_gazebo_plugin airport.launch

After cloning the repo, you are required to delete the 'devel' and 'build' directory inside the catkin_ws in order to use catkin_make in your own device. 
'devel' and 'build' file will be created again after you run catkin_make.
```
## What is included:
- A Pedstrain simulator from https://github.com/srl-freiburg/pedsim_ros
- Nothing more for now

## Installed Pakcages:
- Pedestrian Simulator (ped_sim)
- Turtlebot3
## Branch Info:
- This is the branch for World and Robot Model/Sensor Development
