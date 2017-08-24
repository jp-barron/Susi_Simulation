A program to simulate electrons and gammas from a radioactive source (e.g. SR-90, Am-241) interacting with pixel sensors. Shielding and triggering can be simulated and the setup (e.g. geometry, materials) can be easily changed.

The program uses the GEANT4 framework. 
The corresponding GEANT4 license can be found here:
http://geant4.web.cern.ch/geant4/license/LICENSE.html

This software is under heavy development right now. Nevertheless only compiling revisions are comitted to the master branch. The Readme/wiki will be build soon. So far: Please contact the author if you have any questions.

---- Changes by Jared Barron in July and August 2017
-Changed geometry of simulation. Changed source shield into a G4Tub describing the housing of the Sr-90 source used in the SSD group at CERN. Added a volume for the plexiglass platform holding up collimator and source. Added a volume for PCB holding silicon sensor. Added a volume for bracket holding PCB. Changed trigger to a cylinder, as in the SSD group's setup. 

-Added a tree storing information about triggering - TriggerBool records 1 or 0 depending on whether the particle entered the volume of the trigger. TriggerEDep stores the amount of energy deposited in the trigger. SensorEDep stores the energy deposited in the sensor. 

-Attempted to get energy of particles at time of entering the trigger volume. Only works if the tracks are killed immediately after entering the trigger (reason unknown). This prevents simulating energy deposition in the trigger. 

-The software is used as follows: 
1. In your local GEANT4 installation, /$GEANT4_INSTALL/bin/, run . ./gean4.sh
2. In your build directory of SourceSim, run ./SourceSimulation
3. In the entry field at the bottom of the window that opens up, GEANT4 interactive commands can be input, and macro files can be executed with /control/execute/ your_mac.mac
4. run.mac executes the simulation. Edit it to choose histogram binning, source properties, number of events in the run, etc. 
5. Output root filename can be changed in /SourceSim/src/HistoManager.cc 
6. Detector geometry, including sensor thickness, can be changed in /SourceSim/src/DetectorConstruction.cc
7. To view the geometry, in the geant4 interactive window, you can run
	/vis/open OGL
	/run/initialize
	/control/execute vis.mac

