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
All the statements in order to let the robot moves, avoiding the walls, are made in the control node, whereas the user node should constantly wait for an input by the user, which can either ask to increment or decrement the velocity, or to put the robot in the initial position (thanks to the std_srvs/Empty service). 
Possible commands that the user can make:
* A = accelerate the robot
* D = decelerate the robot
* R = reset its position

In the boolean function (in the control node), there might notice Z. Z is importat since will let return false about the boolean function, so the velocity doesn't increment without pressing any key.  

Nodes
-----
### Stage_ros node 

The stage_ros node subscribes to the /cmd_vel topic from the package `geometry_msgs` which provides a `Twist` type message to express the velocity of the robot in free space, broken into its linear and angular parts (x,y,z).
The stage_ros node also publishes on the `base_scan` topic, from the package called `sensor_msgs` that provides a `LaserScan`, a laser range-finder.
We had also to call the service `reset_positions` from the `std_srvs` package in order to reset the robot position. In `std_srvs` it is contained the `Empty` type service.

### Control node

In the control node there is the main code of the project. This node handles multiple information, moreover it contains the main structure of the code which allows the robot to avoid hitting wall and drive through the circuit without any problem. Furthermore the node permits to increment/decrement the velocity of the robot and reset its position (through the inputs given from keyboard and "passed" by the custom service `Acceleration.srv` (in the srv folder) that handles two elements: char Kinput, that is the request from the client and float32 value, that is the response from the server; in fact the control node is the server node that receives the request from the user node (client node)). 
In the `base_scan` topic, which provides datas about the laser that scans the surrounding environment, there is the type message `sensor_msgs/LaserScan`. The topic provides an array, which returns the distances between the robot and the obstacles; every distance is given by the array ranges[i] (i = 0,...,720) and it is computed considering each of 721 section in which the vision of the robot is divided, since the vision of the robot is included in a spectrum range of 180 degrees in front of it and expressed in radiant. I have separated 3 big subsections (right, left and in front of the robot), inside the 0-720 spectrum, for the vision of the robot and i have computed the minimum distance between the robot and the obstacle for each subsection, in order to implement the similar logic seen in the previous assignment. This is the function:

```cpp
float Robot_Distance(int min_value, int max_value, float distance_obs[]){
	
	// setting a general distance value for comparison
	
	float distance_value = 40.0;
	
	// Thanks to this for, we will obtain the minimum distance from an obstacle
	// inside that particular subsection	

	for(int i = min_value; i <= max_value; i++){
		
		if(distance_obs[i] <= distance_value){
		
			distance_value = distance_obs[i];
		}
	}
	
	return distance_value;
}
```
These are the minimun distances for each subsection (done in the RobotCallback function):

```cpp
	float min_right_dist = Robot_Distance(0, 105, laser_scanner);
	float min_front_dist = Robot_Distance(305, 405, laser_scanner);
	float min_left_dist = Robot_Distance(615, 715, laser_scanner);
}
```
As i said before the node permits to increment/decrement the velocity of the robot and reset its position and it is done thanks to this "switch", inside the boolean function, that handles the request coming from the user node (no response are sent to the user node) : 

```cpp
switch(req.Kinput){
	
	// I made a switch in order to discriminate the different buttons we press in the user_node
	// If "A" is pressed --> increment by 0.5 the plus variable
	// If "D" is pressed --> decrement by 0.5 the plus variable
	// If "R" is pressed --> the control_node will call the /reset_positions service and the robot will reset its position back to the start
	// Z is importat since will let return false about the boolean function, so the velocity doesn't increment without pressing any key. 
	// The terminal will let us know if another button is pressed
	
		case('a'):
			plus += 0.5;
			req.Kinput = 'z';
			
			// Thanks to ROS_INFO, on the terminal there will be printed the increment of acceleration
	
			ROS_INFO("\nIncrement of acceleration :@[%f]", plus);
		break;
		
		case('d'):
			plus -= 0.5;
			req.Kinput = 'z';
			ROS_INFO("\nIncrement of acceleration :@[%f]", plus);
		break;
		
		case('r'):
			ros::service::call("/reset_positions", res_pos);
			req.Kinput = 'z';
		break;
		
		case('z'):
			return false;
		break;
		
		default: 
			std::cout << "Wrong command!\n";
			fflush(stdout);
		break;
	}
```
### plus is a globar variable that increment/decrement by 0.5 each time a button (A/D) is pressed by the user. 

These are the simple statements in order to let the robot drive easily through the circuit:
The following control is implemented in the RobotCallback function that will be called whenever a message is posted on the `base_scan` topic.

```cpp
if(min_front_dist < 1.6){
			
    	if(min_right_dist < min_left_dist){
    	
    	// The robot will turn right if the minimum distance from the wall on the left is 
	// greater than the distance of the wall on the right
    	
    		my_vel.angular.z = 1.0;
    		my_vel.linear.x = 0.1;
		}
		
		else if(min_right_dist > min_left_dist){
		
    	// The robot will turn left if the minimum distance from the wall on the right is 
		// greater than the distance of the wall on the left
		
			my_vel.angular.z = -1.0;
			my_vel.linear.x = 0.1;
		}
	}
		
	else{
	
	// If the robot doesn't have to make a curve to avoid hitting walls, will go straight.
	
	// The plus variable will increment/decrement if we press "A"/"D" on the user_node
	
	my_vel.linear.x = 1.0 + plus;
	my_vel.angular.z = 0.0;
	
		// Just a simple control to avoid make the robot going backwards.
	
		if(my_vel.linear.x < 0.0){
			
			my_vel.linear.x = 0.0;
			my_vel.angular.z = 0.0;
		}
	}
```
#### NB: The parameters (such as 0.6, 1, -1) have been tested pesonally and i consider these as the best ones in order to let the robot makes various loops clocwisely. BUT a problem may occurs if the user increments by A LOT (!!!!) the speed of the robot--> it may crush (or having problems on turning in the right direction, going counterclockwise).

The node also behaves like a PUBLISHER since it publish on the topic `/cmd_vel` the type message `cmd_vel geometry_msgs/Twist`, that regards the velocity of the robot, broken in its angular and linear parts (x,y,z). 

### User node

The user node is very important since represents the interface of the user. Through the user node we can increase/decrease the velocity of the robot and reset its position by simple commands:
* A --> accelerate the robot by 0.5
* D --> decelerate the robot by 0.5 
* R --> reset robot position. 

All the commmands are detected from the keyboards thanks to this function:

```cpp
char GETCHAR(void){

	char ch;
	
	std::cout << "ACCELERATE the robot: press A\n";
	std::cout << "DECELERATE the robot: press D\n";
	std::cout << "RESET the robot's position: press R\n";
	
	std::cin >> ch;
	
	return ch;
}
```
The service Acceleration.srv is made like this:

``` xml
     char Kinput
     ---
     float32 value
```
Thanks to this service the request is sent to the control node (the request is a char, the one pressed on keyboard) and the server node (control node) will manage the request using a switch. No response will be sent to the user node (the response should have been a float32 value) since the service operate directly on the control node!

Flowchart
---------

This is an image that show how nodes are connected to each other (thanks to the command rosrun rqt_graph rqt_graph):

<p align="center">
<img src="https://github.com/LoreBene99/RT1_Assignment2/blob/main/images/rosgraph.png" width="700" height="300">
</p>

This is an image that represents how act the custom service: 

<p align="center">
<img src="https://github.com/LoreBene99/RT1_Assignment2/blob/main/images/flowchart.png" width="800" height="450">
</p>

This is a flowchart that represents the behaviour of the robot inside the environment:

<p align="center">
<img src="https://github.com/LoreBene99/RT1_Assignment2/blob/main/images/robotbeh.png" width="600" height="450">
</p>

Conclusions
-----------

I'm satisfied wiht the final result, even if better improvements can be done. I have learned a lot of things about ROS logic, how nodes communicate to each other and how datas and information are exchanged. The robot makes 1++ lap inside the Monza's circuit and works very clearly, BUT of course some improvenments can be done:
* At high speeds the robot could crush, maybe it is possible to create some statements in order to avoid it, even it is quite normal for this to happen.
* The robot could have some problems (very rare) in the turning hot spots (like the first curve) and could turn itself in a wrong way, miscalculating the turning decision, going on the opposite direction (*This is a very rare case, but at high speeds this problem can occurs a FEW more times). Maybe there might be better paramters' values or some particular statements that can be created to avoid this problematic*).

I decided also to implement only 2 nodes. If not i should have implemented the server nodes that offers the service about increasing/decreasing the velocity and resetting the robot position (i put all this directly inside the server nodes in our case, that is the control node).






	



