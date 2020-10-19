## Installation

  Some basic guidelines on the installation
```
git clone https://github.com/iamliguangming/Guidedog
cd Guidedog
git checkout world_branch
git submodule update --init --recursive
cd catkin_ws
catkin_make
source devel/setup.bash
roslaunch pedsim_simulator airport_demo.launch 
roslaunch pedsim_gazebo_plugin airport.launch
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

## Installed Pakcages:
- Pedestrian Simulator (ped_sim)
- Turtlebot3
## Branch Info:
- This is the branch for World and Robot Model/Sensor Development
