/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2015, University of Colorado, Boulder
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Univ of CO, Boulder nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Dave Coleman
   Desc:   Records a ros_control ControllerState data to CSV for Matlab/etc analysis
*/

#include<ros_control_boilerplate/controller_state_to_csv.h>

// Basic file operations
#include <iostream>
#include <fstream>

namespace ros_control_boilerplate
{

ControllerStateToCSV::ControllerStateToCSV(bool verbose, const std::string& topic)
  : verbose_(verbose)
{
  // State subscriber
  state_sub_ = nh_.subscribe<control_msgs::JointTrajectoryControllerState>(topic, 1, &ControllerStateToCSV::stateCB, this);

  // Wait for states to populate
  //ros::Duration(1).sleep();
  //ros::spinOnce();

  ROS_INFO_STREAM_NAMED("controller_state_to_csv","ControllerStateToCSV Ready.");
}

void ControllerStateToCSV::stateCB(const control_msgs::JointTrajectoryControllerState::ConstPtr& state)
{
  controller_state_ = *state;
}

// Start the data collection
void ControllerStateToCSV::startRecording(const std::string& file_name)
{
  file_name_ = file_name;

  // Reset data collections
  controller_states_.clear();
  timestamps_.clear();

  // Start sampling loop
  ros::Duration update_freq = ros::Duration(1.0/RECORD_RATE_HZ);
  non_realtime_loop_ = nh_.createTimer(update_freq, &ControllerStateToCSV::update, this);
}

void ControllerStateToCSV::update(const ros::TimerEvent& e)
{
  if (first_update_)
  {
    // Check if we've recieved any states yet
    if (controller_state_.joint_names.empty())
    {
      ROS_WARN_STREAM_THROTTLE_NAMED(2, "update","No states recieved yet");
      return;
    }
    first_update_ = false;
  }
  else
    ROS_INFO_STREAM_THROTTLE_NAMED(2, "update","Updating with period: "
                                   << ((e.current_real - e.last_real)*100) << " hz" );

  // Record state
  controller_states_.push_back(controller_state_);

  // Record current time
  timestamps_.push_back(ros::Time::now());
}

void ControllerStateToCSV::stopRecording()
{
  non_realtime_loop_.stop();
  writeToFile();
}

bool ControllerStateToCSV::writeToFile()
{
  if (!controller_states_.size())
  {
    ROS_ERROR_STREAM_NAMED("controller_state_to_csv","No controller states populated");
    return false;
  }

  std::ofstream output_file;
  output_file.open (file_name_.c_str());

  // Output header -------------------------------------------------------
  output_file << "timestamp,";
  for (std::size_t j = 0; j < controller_states_[0].joint_names.size(); ++j)
  {
    output_file << controller_states_[0].joint_names[j] << "_desired_pos,"
                << controller_states_[0].joint_names[j] << "_desired_vel,"
                << controller_states_[0].joint_names[j] << "_actual_pos,"
                << controller_states_[0].joint_names[j] << "_actual_vel,"
                << controller_states_[0].joint_names[j] << "_commanded_vel,";
  }
  output_file << std::endl;

  // Output data ------------------------------------------------------

  // Subtract starting time
  //double start_time = controller_states_[0].header.stamp.toSec();
  double start_time = timestamps_[0].toSec();

  for (std::size_t i = 0; i < controller_states_.size(); ++i)
  {
    // Timestamp
    output_file << timestamps_[i].toSec() - start_time << ",";

    // Output entire state of robot to single line
    for (std::size_t j = 0; j < controller_states_[i].joint_names.size(); ++j)
    {
      // Output State
      output_file << controller_states_[i].desired.positions[j] << ","
                  << controller_states_[i].desired.velocities[j] << ","
                  << controller_states_[i].actual.positions[j] << ","
                  << controller_states_[i].actual.velocities[j] << ","
                  << controller_states_[i].error.velocities[j] << ",";
    }

    output_file << std::endl;
  }
  output_file.close();
  ROS_INFO_STREAM_NAMED("controller_state_to_csv","Wrote to file " << file_name_);
  return true;
}

} // end namespace
