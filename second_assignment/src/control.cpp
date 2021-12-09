//--------------------- LIBRARIES ----------------------------------------------------------------------------------------------------

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h" 
#include "geometry_msgs/Twist.h" 
#include "std_srvs/Empty.h"
#include "second_assignment/Acceleration.h"

//------------------------------------------------------------------------------------------------------------------------------------

// The control_node is a publisher since we have to pusblish the velocity of the robot in the 
// cmd_vel topic

ros::Publisher pub;

// This variable "plus" is very important since is the main element 
// that allows us to increment/decrement the velocity of the robot (see below to 
// understand how it is implemented).

float plus = 0.0;

//--------------------- FUNCTIONS ----------------------------------------------------------------------------------------------------

// I can make a function that will obtain the minimum distance from
// an obstacle inside a subsection of the the spectrum ranges[0]-ranges[720].
// I will pass the 2 values that describes my range and the array of distances about that range.

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

// We need to initilize res_pos since we have to make the robot stop with "R"
// thanks to the reset position service.

std_srvs::Empty res_pos;

// We introduce a boolean function to handle the request input coming from the user_node
// and this is allowed by the custom service we had created "Acceleration.srv" that acts as a bridge
// between the nodes. 

bool ServerCallback(second_assignment::Acceleration::Request &req, second_assignment::Acceleration::Response &res){
	
	// The control_node knows what client is passing thanks to Kinput. I had associate Kinput with input from Keyboard
	
	switch(req.Kinput){
	
	// I made a switch in order to discriminate the different buttons we press in the user_node
	// If "A" is pressed --> increment by 0.5 the plus variable
	// If "D" is pressed --> decrement by 0.5 the plus variable
	// If "R" is pressed --> the control_node will call the reset service and the robot will reset its position back to the start
	// If "Z" is pressed --> close user_node
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
	
	res.value = plus;
		    
	return true;
}

// Function RobotCallback() to handle the datas about /base_scan topic 

void RobotCallback(const sensor_msgs::LaserScan::ConstPtr& msg){ 

	// We have to publish the velocity on topic cmd_vel.
	
	geometry_msgs::Twist my_vel; 
	
	// Initilize an empty arrey in order to fullfil it with the ranges[i]'s values
	
	float laser_scanner[721];
	
	for(int i = 0; i < 722; i++){
	
		laser_scanner[i] = msg->ranges[i];
	}
	
	// We take the minimum distance of the robot from the wall in each subsections
	
	float min_right_dist = Robot_Distance(0, 105, laser_scanner);
	float min_front_dist = Robot_Distance(305, 405, laser_scanner);
	float min_left_dist = Robot_Distance(615, 715, laser_scanner);
	
	// As we have done in the previous assignement, the robot will avoid going against the 
	// obstacles thanks to these simply statements.
	
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
	
	// Publish my_vel on cmd_vel (both angular and linear of my_vel since i used geometry_msgs::Twist my_vel and
	// i associate my_vel to linear and angular velocity)
		
	pub.publish(my_vel);					
}

//--------------------------------------------- MAIN ---------------------------------------------------------------------------------

int main (int argc, char **argv){

	// Initialize the node, setup the NodeHandle for handling the communication with the ROS
	//system

	ros::init(argc, argv, "control_node");
	ros::NodeHandle nh;
	
	// We have to introduce this line since the control_node acts like a server (considering the custom service Acceleration it provides {to itslef})
	
	ros::ServiceServer service =  nh.advertiseService("/accelaration", ServerCallback);
	
	// Since we have to publish my_vel in the cmd_vel topic we are goint to instert this important line:
	
	pub = nh.advertise<geometry_msgs::Twist> ("/cmd_vel", 1);

	// Define the subscriber to /base_scan topic
	
	ros::Subscriber sub = nh.subscribe("/base_scan", 1, RobotCallback);
	
	ros::spin();
	
	return 0;
}
