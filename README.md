Overview
--------------------------
The purpose of this project is to read data from an 8-channel hydrophone using the
cDAQ-9136 and NI-9223. The device will be running "headless" on a kayak.


General Order of Operations
--------------------------
The device operates like so.
	- Boots upon power-up
	- launches a init script that launches the C++ program
	- C++ program reads config.ini to determine its recording parameters
	- Program sleeps until desired start time
	- Device records data according to config.ini
	- Device writes data out for each of the 8 channels as .wav files (usings slots 1 & 2,
	  with channels 0-3 being 0-3 on slot 1, channels 4-7 being 0-3 on slot 2)

The device can be configured to record at different rates (up to 1 MHz), different lengths, and at different times.
This can be achieved by manually editting the config.ini file or by using a program to do so (not yet created).


Project Setup
--------------------------
To setup the project use the following link: https://forums.ni.com/t5/NI-Linux-Real-Time-Documents/Example-Using-the-DAQmx-API-on-NI-Linux-Real-Time-Systems/ta-p/3536811


TODO List
--------------------------
	- Add program to edit config programs (quick c++ program and hacked together windows python gui?)
	- Create init script
	- Add NTP client to device to increase time accuracy (UA internet may be an issue for downloading package)
	- Add a license
	- Add more details to this README (SSH and FTP basics for getting .wav files, memory constraints, 
	  startTime specifics, file locations on cDAQ-9136, project setup details)