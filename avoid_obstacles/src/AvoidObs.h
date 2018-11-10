#ifndef AvoidObs_H
#define AvoidObs_H

//ROS Includes
#include <ros/ros.h>
#include <nav_msgs/OccupancyGrid.h>
#include <nav_msgs/Path.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include <tf/transform_listener.h>

#include "Astar.h"
#include "PotentialFields.h"
//#include <std_msgs/String.h>


class AvoidObs
{
    public:
		AvoidObs();
        ~AvoidObs();
        
        //returns parameter rate_ in hz
        // used to define ros::Rate
        double get_plan_rate();
        bool update_plan();
        
    private:
        void scanCallback(const sensor_msgs::LaserScan& scan);
        void odomCallback(const nav_msgs::Odometry& odom);
        void goalCallback(const geometry_msgs::PoseStamped& data);
        
        void update_cell(float x, float y, int val);
        
        bool get_map_indices(float x, float y, int& ix, int& iy);
        int get_cost(int ix, int iy);

        ros::NodeHandle nh_;
        ros::NodeHandle nh_p;
        ros::Publisher costmap_pub_, pf_obs_pub_;
        ros::Publisher path_pub_, cmd_pub_;
        ros::Subscriber scan_sub_, odom_sub_, goal_sub_;
        
        Astar astar;
        PotentialFields pf;

        unsigned num_obs_cells; //number of detections given in 0x751
        
        geometry_msgs::Pose map_pose, pfObs_pose;
        nav_msgs::OccupancyGrid costmap, pfObs;
        nav_msgs::Path path;
        geometry_msgs::Pose bot_pose, goal_pose;
        float bot_yaw;
        
        tf::TransformListener listener;

        //parameters
        double plan_rate_; //Default 1 Hz, how often we use A* to update path plan
        double map_res_; //Default 0.5 meters
        int n_width_, n_height_;
        double max_range_;
        double plan_range_;
        int clear_decrement_, fill_increment_;
        bool use_Astar_, use_PotFields_;
};

#endif