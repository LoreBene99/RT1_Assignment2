Second Assignment of Research Track 1 
================================
## Professor : Carmine Recchiuto, University of Genoa

The project, given by the Professor Recchiuto, regards the use of a Robot Simulator (ROS) in order to develop our skills in ROS' environment. The robot is inside a scaled reproduction of the Monza's circuit. The main gol of the project is to drive the robot inside the circuit and it must avoid hitting walls. Also, there must be a User-Interface node thanks to an external user can increment/decrement the velocity of the robot and can also reset its position back from the start. In order to do it i need to create to nodes:
* User node 
* Control node 
Thanks to these nodes we can fullfil the requirenments. 

Installing and Running
----------------------
The Robot Operating System (ROS) is a set of software libraries and tools that help you build robot applications. In order to install and run the project you have to create your own ROS workspace and in the src folder you have to:
* Download the second_assignment folder and put it inside the src folder, since the second_assignment folder represents the ROS package in which there are the nodes.
* Do catkin_make in the ROOT FOLDER OF YOUR WORKSPACE (Catkin is the official build system of ROS and the successor to the original ROS build system, rosbuild).

Then in order to run the project i have created a ROS launch file, named file.launch, contained in the launch_file folder : 

```xml
<launch>
<node name="stageros" pkg="stage_ros" args="$(find second_assignment)/world/my_world.world" type="stageros"/>
<node name="control_node" pkg="second_assignment" output="screen" type="control_node"/>
<node name="user_node" pkg="second_assignment" output="screen" type="user_node"/>
</launch>
```
To launch the entire project at once, avoding running the single nodes, type: 
#### roslaunch second_assignment file.launch

Introduction
------------
The environment in which the robot moves is :

<p align="center">
<img src="https://github.com/LoreBene99/RT1_Assignment2/blob/main/images/map.png" width="550" height="400">
</p>

This environment is provided by the stage_ros node and it is inside the my_world.world file in the folder world. The robot will know everything about the environment thanks to the stage_ros node.
The robot should go clockwise.
All the statements in order to let the robot moves, avoiding the walls, are made in the control_node, whereas the user_node should constantly wait for an input by the user, which can either ask to increment or decrement the velocity, or to put the robot in the initial position (thanks to the std_srvs/Empty service). 
Possible commands that the user can make:
* A = accelerate the robot
* D = decelerate the robot
* R = reset its position

Also if you press Z the user_node will close. 

Nodes
-----
### stage_ros node 

The stage_ros node subscribes to the /cmd_vel topic fromd the package `geometry_msgs` which provides a `Twist` type message to express the velocity of the robot in free space, broken into its linear and angular parts (x,y,z).
The stage_ros node also publishes on the `base_scan` topic, from the package called `sensor_msgs` that provides a `LaserScan`, a laser range-finder.
We had also to call the service `reset_position` from the `std_srvs` package in order to reset the robot position. In `std_srvs` it is contained the `Empty` type service.



