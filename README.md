## Installation

Some basic guidelines on the installation
```
git clone https://github.com/iamliguangming/Guidedog 
cd Guidedog
git submodule update --init --recursive
source catkin_ws/devel/setup.bash
roslaunch pedsim_simulator airport_demo.launch 
roslaunch pedsim_gazebo_plugin airport.launch

TRY
```
## What is included:
- A Pedstrain simulator from https://github.com/srl-freiburg/pedsim_ros
- Nothing more for now

## Installed Pakcages:
- Pedestrian Simulator (ped_sim)
- Turtlebot3
## Branch Info:
- This is the branch for World and Robot Model/Sensor Development
