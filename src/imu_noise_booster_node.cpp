#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <random>

std::default_random_engine randomEngine;
double linearAccelerationNoiseStd;
std::normal_distribution<double> linearAccelerationNoiseDistribution;
double angularSpeedNoiseStd;
std::normal_distribution<double> angularSpeedNoiseDistribution;
double maxLinearAcceleration;
double maxAngularSpeed;
ros::Publisher pub;

void callback(const sensor_msgs::Imu& msgIn)
{
	sensor_msgs::Imu msgOut = msgIn;
	msgOut.linear_acceleration.x = std::min(std::max(msgIn.linear_acceleration.x + linearAccelerationNoiseDistribution(randomEngine), -maxLinearAcceleration), maxLinearAcceleration);
	msgOut.linear_acceleration.y = std::min(std::max(msgIn.linear_acceleration.y + linearAccelerationNoiseDistribution(randomEngine), -maxLinearAcceleration), maxLinearAcceleration);
	msgOut.linear_acceleration.z = std::min(std::max(msgIn.linear_acceleration.z + linearAccelerationNoiseDistribution(randomEngine), -maxLinearAcceleration), maxLinearAcceleration);
	msgOut.angular_velocity.x = std::min(std::max(msgIn.angular_velocity.x + angularSpeedNoiseDistribution(randomEngine), -maxAngularSpeed), maxAngularSpeed);
	msgOut.angular_velocity.y = std::min(std::max(msgIn.angular_velocity.y + angularSpeedNoiseDistribution(randomEngine), -maxAngularSpeed), maxAngularSpeed);
	msgOut.angular_velocity.z = std::min(std::max(msgIn.angular_velocity.z + angularSpeedNoiseDistribution(randomEngine), -maxAngularSpeed), maxAngularSpeed);
	pub.publish(msgOut);
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "imu_noise_booster_node");

	ros::NodeHandle nodeHandle;
	ros::NodeHandle privateNodeHandle("~");

	privateNodeHandle.param<double>("linear_acceleration_noise_std", linearAccelerationNoiseStd, 0.0f);
	linearAccelerationNoiseDistribution = std::normal_distribution<double>(0.0f, linearAccelerationNoiseStd);
	privateNodeHandle.param<double>("angular_speed_noise_std", angularSpeedNoiseStd, 0.0f);
	angularSpeedNoiseDistribution = std::normal_distribution<double>(0.0f, angularSpeedNoiseStd);
	privateNodeHandle.param<double>("max_linear_acceleration", maxLinearAcceleration, 1000);
	privateNodeHandle.param<double>("max_angular_speed", maxAngularSpeed, 1000);

	pub = nodeHandle.advertise<sensor_msgs::Imu>("imu_out", 10);

	ros::Subscriber sub = nodeHandle.subscribe("imu_in", 10, callback);

	ros::spin();

	return 0;
}

