//--------------------- LIBRARIES ----------------------------------------------------------------------------------------------------

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Twist.h"
#include "second_assignment/Acceleration.h"  
#include "std_srvs/Empty.h"

//------------------------------------------------------------------------------------------------------------------------------------

// Since the user_node will act like a client, we have to initilize it

ros::ServiceClient client1;

//--------------------- FUNCTION ----------------------------------------------------------------------------------------------------- 

// We will take the input from the keyboard thanks to GETCHAR() function

char GETCHAR(void){

	char ch;
	
	std::cout << "ACCELERATE the robot: press A\n";
	std::cout << "DECELERATE the robot: press D\n";
	std::cout << "RESET the robot's position: press R\n";
	
	std::cin >> ch;
	
	return ch;
}

// Callback function to handle datas

void Callback(const sensor_msgs::LaserScan::ConstPtr& msg){

	// Associate acc_srv to acceleration service
	
	second_assignment::Acceleration acc_srv; 
	
	// We let the program knows that input variable contains the character 
	// pressed on the keyboard
	
	char input = GETCHAR();
	
	// The input we take from keyboard is associated to the "request" handle by the service 
	// and sent to the control_node
	
	acc_srv.request.Kinput = input;
	
	// Before calling the service just wait that is ready
	
	client1.waitForExistence();
	client1.call(acc_srv);
}

//--------------------------------------------- MAIN ---------------------------------------------------------------------------------

int main (int argc, char **argv){

	// Initialize the node, setup the NodeHandle for handling the communication with the ROS //system
	
	ros::init(argc, argv, "user_node"); 
	ros::NodeHandle nh;
	  
	// Make the connections between client and service
	
	client1 = nh.serviceClient<second_assignment::Acceleration>("/accelaration");
	
	ros::Subscriber sub = nh.subscribe("/base_scan", 1, Callback);

	ros::spin();
	
	return 0;
}
