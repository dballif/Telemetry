# Telemetry
This project is designed as a way to read multiple sensors attached to a RaspberryPi. Eventually this project will use MQTT protocol to transport this information to a SQLite database for storage. It was originally designed to measure the environmental statistics such as temperature and humidity at various points in my yard, a beehive, and the garden.

## Setup
This project was built on a RaspberyPi Zero that had various environmental sensors attached. At this time, the project only supports "w1" style sensor inputs but in the future I hope to incorporate more types.

### Necessary Libraries
Libconfuse - Used to read configuration variables from a file
spdlog - Used to make logging easier
g++ - Used for compiling

## Configuration File
The program will look for a configuration file with the title "telemetry.cfg". Each sensor can be included in this configuration file. An example configuration file can be found in the "doc" directory. 

There are also some default values put in if no configuration file is found. 

You can specify a new configuration file by passing -f <file>.

## To Build
To compile use command: g++ telemetry.cc -L/usr/local/lib -lconfuse -o telemetry.out

(This will link the libconfuse library, but make sure it the -L points to wherever yoru libraries are setup)

## To Run
The compiled file is named telemetry.out and can be run without any arguments. However, there are a few arguments that can be given to it using the following flags:

-h: Displays the help menu  
-v: Displays the version  
-f <file>: Specifies the configuration file to use  
-d <trace,debug,error,warn,info>: Specify the log level (Info is set by default)

## Expected Behavior
When the program is running correctly, it should continually output the payload that would be sent to the database in the info log (this is the default). An example of this output is:
 
"[2022-01-06 19:56:06.849] [main] [info] Next Payload: Thu Jan  6 19:56:06 2022,beehive,Nursery,18.312000"
 
 Obviously if deeper logs are turned on you will see a lot more output.

## Doxygen Documentation
Throughout most of the project Doxygen comment notation has been used. Further documentation in this format will be added at a later time.
 
## Future Improvments
The next step is to finish adding the MQTT pieces to transmit all of this data to a SQLite database to be stored for further use.
It would then be cool to write a webapp or mobile app to view the data and what not.
More sensor types would also be good to have

## Udacity Rubric Points
Here are the rubric criteria that are addressed in this project:

|                                         Criteria                                         |     File     |              Line             |
|:----------------------------------------------------------------------------------------:|:------------:|:-----------------------------:|
| The project demonstrates an understanding of C++ functions and control Structures        | telemetry.cc |       26,51,133,180,275       |
| Project reads data from a file and process the data                                      | telemetry.cc |             97-123            |
| The project accepts input from a user as part of the necessary operation of the program. | telemetry.cc |             52-73             |
| The Project uses object oriented programming techniques.                                 |  sensors.hh  |               22              |
| Classes use appropriate access specifiers for class members                              |  sensors.hh  |             23,31             |
| Classes abstract implementation details from their interfaces.                           |  sensors.hh  | Comments throughout the class |
| Classes encapsulate behavior.                                                            |  sensors.hh  |             34-46             |
