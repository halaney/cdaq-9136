Overview
--------------------------
The purpose of this project is to read data from an 8-channel hydrophone using the
cDAQ-9136 and NI-9223. The device will be running "headless" on a kayak. This is
achieved through two separate programs, ConfigWriter and reader. ConfigWriter
updates the config file used by reader. reader waits until the given start time
and records data based on the config file. The reader program current runs as an
init process (therefore starts at power-up).


Basic Usage
--------------------------
Below is an example of how to use the two programs
 1. SSH into the cDAQ-9136 and login
 2. To update the configuration for recording use the ConfigWriter program
    ```bash
    cd /home/admin/ConfigWriter
    ./ConfigWriter
    ```
 3. To reload the configuration used by reader either restart the machine or use
     ```bash
     /etc/init.d/reader stop
     /etc/init.d/reader start
     ```
     This just stops and starts the reader program
 4. Once the reader program has recorded the data its output will be stored in
    /home/admin/reader/output/. It can be retrieved using a SFTP client such as
    filezilla. Be sure to delete the files after retrieving them to free up space

More details can be found in UserGuide.txt

Project Setup for Development
--------------------------
To setup the project for development use the following link:
https://forums.ni.com/t5/NI-Linux-Real-Time-Documents/Example-Using-the-DAQmx-API-on-NI-Linux-Real-Time-Systems/ta-p/3536811

Some other useful links:
http://www.ni.com/product-documentation/2835/en/
http://zone.ni.com/reference/en-XX/help/370471AA-01/
http://forums.ni.com/t5/NI-Linux-Real-Time-Documents/Tutorial-Installing-Startup-Scripts-on-NI-Linux-Real-Time/ta-p/3527257

Refer to the DeveloperGuide.txt for more information
