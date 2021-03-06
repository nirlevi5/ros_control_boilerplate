# ros_control Boilerplate

Provides a simple simulation interface and template for setting up a hardware interface for ros_control. The idea is you take this as a starting point for creating your hardware interfaces, and it is needed because [ros_control documentation](http://wiki.ros.org/ros_control) is sparse. This boilerplate demonstrates:

 - Creating a hardware_interface for multiple joints for use with ros_control
 - Position Trajectory Controller
 - Control of 2 joints of the simple robot "RRBot" pictured below
 - Loading configurations with roslaunch and yaml files
 - Generating a random trajectory and sending it over an actionlib interface
 - Partial support of joint mode switching (but this feature has not been release with ros_control)
 - Visualization in Rviz

Developed by [Dave Coleman](dav.ee) at the University of Colorado Boulder

<img align="right" src="https://raw.githubusercontent.com/davetcoleman/ros_control_boilerplate/indgo-devel/resources/screenshot.png" />
                        

## Video Demo

See [YouTube](https://www.youtube.com/watch?v=Tpj2tx9uZ-o) for a very modest video demo.

## Install

This package depends on [gazebo_ros_demos](https://github.com/ros-simulation/gazebo_ros_demos) for its ``rrbot_description`` package, so be sure to ``git clone`` that along with this package and build in your catkin workspace.

## Run Demo

This package is setup to run the "RRBot" two joint revolute-revolute robot demo. To run its ros_control hardware interface, run:

    roslaunch ros_control_boilerplate rrbot_hardware.launch

To visualize its published ``/tf`` coordinate transforms in Rviz run:

    roslaunch ros_control_boilerplate rrbot_visualize.launch

To send a random, dummy trajectory to execute, run:

    roslaunch ros_control_boilerplate rrbot_test_trajectory.launch

## Customize

To test this as a simulation interface for your robot:

 - Rename all files in ``/launch`` to use your robot name
 - Find-replace all strings in ``/launch`` from "rrbot" to your robot name
 - Customize ``/config/rrbot_controllers.yaml`` to have joints corresponding to your robot's URDF

To test this as a hardware interface for your robot, you'll also want to rename the files in ``/src`` and find-replace the string "generic" to your robot's name. Then add the necessary code to communicate with your robot via USB/serial/ethernet/etc. Feel free to PR this package with better step by step instructions for this.

## Setting an Initial Position, Using with MoveIt!

If you need your robot to startup at a particular position in simulation, or you would like to use this funcitonality to simulate your robot with MoveIt!, see the downstream package (it depends on this package) [moveit_sim_controller](https://github.com/davetcoleman/moveit_sim_controller)

# Other Helper Tools - Recording to CSV

Write to file

    rosrun ros_control_boilerplate controller_state_to_csv SAVE_TO_FILE_PATH CONTROLLER_STATE_TOPIC TIME_TO_RECORD

Read from file

    rosrun ros_control_boilerplate csv_to_controller READ_FROM_FILE_PATH CONTROLLER_STATE_TOPIC TIME_TO_RECORD

## Limitations

 - Does not implement joint limits, estops, transmissions, or other fancy new features of ros_contorl
 - Does not have any hard realtime code, this depends largely on your platform, kernel, OS, etc
 - Only position control is fully implemented, though some code is in place for velocity and effort control

## Contribute

Please add features, make corrections, and address the limitations above, thanks!
