#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    client.call(srv);
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    for (int i = 0; i < img.height * img.step; i+=3)
    {
    	if(img.data[i] == 255 && img.data[i+1] == 255 && img.data[i+2] == 255)
    	{
    	    int spot = i % img.step;
    	    if (spot < img.step/3)
    	    {
    	    	drive_robot(0.2,0.2);
    	    }
    	    else if (spot >= img.step/3 && spot < 2*img.step/3)
    	    {
    	    	drive_robot(0.2,0);
    	    }
    	    else if (spot >= 2*img.step/3 && spot < img.step)
    	    {
    	    	drive_robot(0.2,-0.2);
    	    }
    	    else
    	    {
    	    	drive_robot(0,0);
    	    }
    	    break;
    	}
    	
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
