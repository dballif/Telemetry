/**
 * @file sensors.hh
 * @author Daniel Ballif (ballifdaniel@gmail.com)
 * @brief This file contains the sensors class and other important pieces to use the sensors.
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <string>
#include <unistd.h>
#include <fstream>
#include <ctime>
#include <poll.h>
#include <iostream> //Remove when done with cout

using namespace std;

/**
 * @brief Create the sensor class which allows us to load up an object with all the important pieces. Each sensor will have it's own object and these will be fed to the sensArray.
 */
class sensor {
private:
    string s_name;
    string s_module;
    string s_addr;
    string s_type;
    int s_delay;
    string s_inputType;

public:
    sensor();
    
    void setAddr(string newAddr){s_addr = newAddr;} //FIXME: Change temperature to type to accomodate all sensors 
    void setModule(string newModule){s_module = newModule;}
    void setType(string newType){s_type = newType;}
    void setName(string newName){s_name = newName;}
    void setDelay(int newDelay) {s_delay = newDelay;}
    void setInputType(string newInputType){s_inputType = newInputType;}

    string getAddr(){return s_addr;}
    string getModule(){return s_module;}
    string getType(){return s_type;}
    string getName(){return s_name;}
    int getDelay(){return s_delay;}
    string getFullAddr(){return "/sys/bus/"+s_inputType+"/devices/"+s_addr+"/"+s_type;}
} ;

sensor::sensor() {}

/**
 * @brief getData takes an instance of the sensor class, extracts the data, and formats it for MQTT use.
 * 
 * @param chosenSensor an instance of the sensor class from which data will be extracted.
 * @return string is the payload that will be sent via MQTT
 */
void getData(sensor chosenSensor,char* sensorData) {
        string convertedData = sensorData;
        string sensorPayload;
        time_t timeAtGet;
        //All the Time Pieces
        struct tm * ti;
        time(&timeAtGet);
        ti = localtime(&timeAtGet);
        char *formatTime = asctime(ti);
        if (formatTime[strlen(formatTime)-1] == '\n') formatTime[strlen(formatTime)-1] = ',';

        //FIXME: Conversion to Celsius instead of mCelsius
        if(chosenSensor.getType()=="temperature"){
            convertedData = to_string(atof(sensorData)/1000);
            mainlog->trace("sensorData converted to " + convertedData);
        }  
        
        //Prepare the payload
        sensorPayload = formatTime + chosenSensor.getModule() + "," + chosenSensor.getName() + "," + convertedData;
        
        mainlog->debug("getData has executed");
        mainlog->debug(sensorData);
        mainlog->info("Next Payload: " + sensorPayload);
        //FIXME: Add the Moquitto stuff here to send off
    }

#endif
