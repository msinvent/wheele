#include "PotentialFields.h"
#include <cmath>

PotentialFields::PotentialFields() :
	c_attr(2.0),
	max_Fattr(10.0),
	c_repel(200.0),
	obs_d0(4.0),
	alpha(0.5)
{
	obs_list.clear();
	bot.x = 0;
	bot.y = 0;
	goal.x = 0;
	goal.y = 0;
}

PotentialFields::~PotentialFields(){}

geometry_msgs::Twist PotentialFields::update_cmd(float bot_yaw)
{
	geometry_msgs::Twist cmd;
	geometry_msgs::Vector3 odom_vel = get_vxvy();
	float des_yaw;

	if(odom_vel.x == 0 && odom_vel.y == 0)
	{
		cmd.linear.x = 0;
		cmd.angular.z = 0;
	}
	else
	{
		des_yaw = atan2(odom_vel.y, odom_vel.x);
	}
	float yaw_error = des_yaw - bot_yaw;
	if(yaw_error > 3.14159)
		yaw_error -= 3.14159*2;
	else if(yaw_error <= -3.14159)
		yaw_error += 3.14159*2;


	cmd.linear.x = 1;
	cmd.angular.z = yaw_error;

	return cmd;
}

geometry_msgs::Vector3 PotentialFields::get_vxvy()
{
	// velocity in the odom frame
	geometry_msgs::Vector3 vel, Fattr, Frepel, Frepel_filt;
	float Fx, Fy;

	vel.x = 0;
	vel.y = 0;

	Fattr = get_Fattr();
	if(Fattr.x == 0 && Fattr.y == 0)
		return vel;
	Frepel = get_Frepel();

	Frepel_filt.x = alpha*Frepel_filt.x + (1-alpha)*Frepel.x;
	Frepel_filt.y = alpha*Frepel_filt.y + (1-alpha)*Frepel.y;

	Fx = Fattr.x + Frepel_filt.x;
	Fy = Fattr.y + Frepel_filt.y;

	float magF_sqd = Fx*Fx + Fy*Fy;
	float magF, dirx, diry;
	if(magF_sqd > 0)
	{
		magF = sqrt(magF_sqd);
		dirx = Fx/magF;
		diry = Fy/magF;
	}
	else
	{
		magF = sqrt(Fattr.x*Fattr.x + Fattr.y*Fattr.y);
		dirx = Fattr.x/magF;
		diry = Fattr.y/magF;
	}

	vel.x = dirx;
	vel.y = diry;

	return vel;
}

geometry_msgs::Vector3 PotentialFields::get_Fattr()
{
	geometry_msgs::Vector3 Fattr;
	Fattr.x = 0;
	Fattr.y = 0;

	float dx = goal.x - bot.x;
	float dy = goal.y - bot.y;
	if(fabs(dx) < 1 && fabs(dy) < 1)
		return Fattr;

	float ax = c_attr * dx;
	float ay = c_attr * dy;
	float mag_sqd = ax*ax + ay*ay;
	if(mag_sqd > max_Fattr*max_Fattr)
	{
		float mag = sqrt(mag_sqd);
		ax = ax/mag*max_Fattr;
		ay = ay/mag*max_Fattr;
	}
	Fattr.x = ax;
	Fattr.y = ay;

	return Fattr;
}
geometry_msgs::Vector3 PotentialFields::get_Frepel()
{
	geometry_msgs::Vector3 Frepel;
	Frepel.x = 0;
	Frepel.y = 0;

	float dx, dy, dmag_sqd, w, Fx, Fy, Fxtan, Fytan;

	for(unsigned k = 0; k<obs_list.size(); ++k)
	{
		Obstacle obs = obs_list[k];
		dx = bot.x - obs.x;
		dy = bot.y - obs.y;
		//forget obstacle radius for now
		dmag_sqd = dx*dx + dy*dy;
		if(dmag_sqd < obs_d0)
		{
			w = c_repel*(1/dmag_sqd - 1/(obs_d0*obs_d0)) / (dmag_sqd*dmag_sqd);
			Fx = w*dx;
			Fy = w*dy;
		}
		else
		{
			Fx = 0;
			Fy = 0;
		}
		// TODO: choose mu based on a-star path
		float mu = 1.0;
		Fxtan = -mu*Fy;
		Fytan = mu*Fx;

		Frepel.x += Fxtan;
		Frepel.y += Fytan;
	}

	return Frepel;
}
