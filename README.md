# reaching-with-avoidance [![Build Status](https://travis-ci.org/robotology-playground/reaching-planner.svg?branch=master)](https://travis-ci.org/robotology-playground/reaching-planner)
===
## Synopsis

- **`reaching-with-avoidance`** is a completed solution to plan and monitor operation of iCub arm. It includes some modules for path planning generation and supervisor the movement of iCub arm, as describing in detail as following:

	- **`reaching-planner`** is a module to generate a motion plan for an arm of the iCub, based on the Incremental Sampling-based Algorithm - RRT* (1), (2). A path plan includes the End-effector and the Elbow in the Forearm, generated by trigger from rpc command. The planner can operate alone or in combination with supervisor. The details of algorithm is described in the [published paper](https://github.com/robotology-playground/reaching-planner/tree/devel#publication).
	- **`reaching-supervisor`** is a module to handle the communication between the `reaching-planner`, the `reactController` and user through yarp communication protocol.

## Dependencies
- [YARP](https://github.com/robotology/yarp)
- [iCub](https://github.com/robotology/icub-main)
- [icub-contrib-common](https://github.com/robotology/icub-contrib-common)
- [reactController](https://github.com/robotology/react-control) 
- [peripersonal-space](https://github.com/robotology/peripersonal-space) (PPS): optional for real-time avoidance behavior
- [objectsPropertiesCollector](http://wiki.icub.org/brain/group__objectsPropertiesCollector.html) (OPC): optional for running with real robot.
- [WYSIWYD](https://github.com/robotology/wysiwyd): optional for running with PPS. In particular, the following modules are required:
	- *opcPopulater*: to create fake objects in OPC
	- *sensationManager*
	- *homeostasis*
	- *allostaticController*
	- *behaviorManager*
	- *kinectServer*
	- *referenceFrameHandler*

## Pipeline

<img src="https://github.com/robotology-playground/reaching-planner/blob/master/misc/planner_supervisor.bmp"/>

## Commands
- For the operation with `reaching-planner` only:
	- By sending commands through yarp rpc port, we can trigger the planner to generate a plan based on the environment perception,
i.e: `yarp rpc /<planner_module_name>/rpc:i`.

	- Command for replan: `replan <deadline>`, with `<deadline>` is the maximum exploration time of each planner.

- For the operation with `reaching-supervisor`:
	- By using rpc service to communicate with the supervisor, i.e: `yarp rpc /<supervisor_module_name>/rpc:i`.

## Applications:
- In simulation:
	- For **reaching only**: [multiple-Cartesian-point_localhost.xml](https://github.com/robotology-playground/reaching-planner/blob/master/app/script/multiple-Cartesian-point_localhost.xml)
	- For **reaching with avoidance**: [planner_reactCtrl_PPS_localhost.xml](https://github.com/robotology-playground/reaching-planner/blob/master/app/script/planner_reactCtrl_PPS_localhost.xml)  
- In robot:
	- For **reaching with avoidance**: [planner_reactCtrl_PPS_icub.xml](https://github.com/robotology-playground/reaching-planner/blob/master/app/script/planner_reactCtrl_PPS_iCub.xml)  
- How to run:
	- Run all modules in scripted application. Remember running the `iCub_SIM` first for simulation case.
	- Connect all modules.
	- Open a terminal and connect to *reactController* and make *reactController* receive references from *supervisor*:  	
		```
		yarp rpc /reactController/rpc:i
		set_streaming_xd
		```
	- Open a terminal and connect to *reaching-supervior* as described in [Commands](https://github.com/robotology-playground/reaching-planner/tree/master#commands) 
	- Query a new motion plan and control the arm for reaching by sending commands to *reaching-supervisor* as described in [Documentation](https://github.com/robotology-playground/reaching-planner/tree/master#documentation). Some useful rpc commands can be listed here:  

		- `run_planner <deadline>`, to plan and move arm to a target in *OPC* that is shown in front of robot. Target can be changed by rpc command: `set_target <name>`
		- `run_planner_pos <Vector of 3D position> <deadline>`, to plan and move arm to a position. 
		- `stop`, to stop moving the arm.
		- `resume`, to continue moving the arm after stopping.

	- In order to see the effect of **PPS**: 
		- Using *opcPopulater* for *virtual objects*. Open a terminal and connect to *opcPopulater*:
		
			```
			yarp rpc /opcPopulater/rpc
			populateSpecific3
			```  
		- Modulate the object `value` with command, with `value` accepting double in range `-0.5 ~ 0`:  
			``` setValueEntity <object_name> <value>```  

		- Then open a terminal and connect to *OPC* to modulate objects' name and position. Following example change the name of `unknown_2` object to `hand`(of other agent) to make *PPS* take into account the hand's position, then move it to change the effect of *PPS* on the motion of arm. The expected effect will be the robot's arm is approaching a desired position then stopping if the partner's hand move to and interfere the motion path of robot's arm.
			```
			yarp rpc /OPC/rpc
			set ((id 2) (name hand))
			set ((id 2) (robot_position_x -0.3) (robot_position_y -0.15) (robot_position_z 0.05) (position_3d (-0.3 -0.15 0.05)))
			set ((id 2) (robot_position_x -0.8) (robot_position_y -0.15) (robot_position_z 0.05) (position_3d (-0.8 -0.15 0.05)))
			```		
		- Or Using `Kinect` to obtain the humans' hands as objects, this requires **kinectServer** and **referenceFrameHandler** from **WYSIWYD**. It is necessary to calibrate the Kinect wrt the icub's vision system. It can be done as following:
			
			```
			1) Delete all but the first line in **referenceFrameHandler.ini**;
			
			2) Start *referenceFrameHandler* and *agentDetector*;
			
			3) Put one object on the table (and only one!), which needs to be visible to both the iCub and the Kinect. Make sure *iol2opc* runs nicely and reports the proper coordinates in iCubGui;
			
			4) Left click on the depth image where the object is;
			
			5) Move the object and repeat 4); do this at least 3 times;
			
			6) Right click on the depth image to saves the calibration;
			
			7) Check the referenceFrameHandler.ini and restart agentDetector. 
			```
			
## Documentation
Online documentation is available here: [http://robotology-playground.github.com/reaching-with-avoidance](http://robotology-playground.github.com/reaching-planner).

## Results
An example of the path generated by the planner is as following:

<p align="center">
  <img src="https://github.com/robotology-playground/reaching-planner/blob/master/misc/planning_result_GUI.bmp"/>
</p>

## License
Material included here is Copyright of *iCub Facility - Istituto Italiano di Tecnologia* and is released under the terms of the GPL v2.0 or later. See the file LICENSE for details.

## Publication
Phuong D.H. Nguyen; Hoffmann, M.; Pattacini, U. & Metta, G. (2016), A fast heuristic Cartesian space motion planning algorithm for many-DoF robotic manipulators in dynamic environments, in 'Humanoid Robots (Humanoids), 2016 IEEE-RAS 16th International Conference on', IEEE, pp. 884-891. [[IEEE Xplore]](http://dx.doi.org/10.1109/HUMANOIDS.2016.7803377) [[postprint]](https://sites.google.com/site/matejhof/publications/NguyenEtAl__CartesianSpacePlanningHumanoids_2016_postprint.pdf?attredirects=0)

## References
[1] S. Karaman and E. Frazzoli, *"Optimal kinodynamic motion planning using incremental sampling-based methods,"* 49th IEEE Conference on Decision and Control (CDC), Atlanta, GA, 2010, pp. 7681-7687.

[2] S. Karaman and E. Frazzoli, *"Incremental Sampling-based Algorithms for Optimal Motion Planning,"* Proceedings of Robotics: Science and Systems, Zaragoza, Spain, 2010.
 
