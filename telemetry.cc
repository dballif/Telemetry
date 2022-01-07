/**
 * @file telemetry.cc
 * @author Daniel Ballif (ballifdaniel@gmail.com)
 * @brief The main file for running the telemetry system between the beehive, digesotr, weather station, and the MQTT Broker
 */

#include "telemetry.hh"
#include "logging.hh"
#include "version.hh"
#include "sensors.hh"
//#include "mqtt.hh"
#include <iostream>
#include <getopt.h>
#include <confuse.h>
#include <vector>
#include <poll.h>

using namespace std;

/**
 * @brief declare the cfg variable to be used for configuration file. This is declared globably.
 */
cfg_t *cfg;

//Functions from header files
void getData(sensor chosenSensor, char tempData);

int main(int argc, char *argv[]) {
    shared_ptr<spdlog::sinks::sink> consoleSink;
    string mainLogLevel, networkLogLevel, sensorsLogLevel;
    const char* cfgFile = "telemetry.cfg";
    string logLevel = "info";
    vector <string> sensors;
    vector <string> serials;
    vector <string> names;
    vector <string> inputTypes;
    string module;
    int cmdlog = 0;
    int c,i,j;
    string nextPayload;
    string data;
    int timeout_msecs = 5000;
    int ret;
    char buf[7];
    int readret;

    /**
     * @brief This loop runs through the arguments given at the command line.
     * The options are h (help), d (log), v (version), f (cfgFile)
     */
    while (1) {
        c = getopt(argc, argv, GETOPT_OPTSTRING);
        if (c == -1)
            break;
        switch (c) {  
        case 'h':
            cout << "-d <log level> -f <cfg file>" << endl; //FIXME: Update help function when finished
            return 0;
        case 'd':
            cmdlog = 1;
            logLevel = optarg;
            break;
        case 'v':
            cout << "Version " << TELEMETRY_VERSION << endl;
            return 0;
        case 'f':
            cfgFile = optarg;
            cout << "Filename: " << cfgFile<< endl;
	        break;
        default:
            break;
	    }
    }
    /**
     * @brief Setup a shared sink for all the different categories. Have to make 
     * that spdlog/sinks/stdout_sinks.h.
     * For setting up the logger see lines 305-308 of logging.cc in apsiproxy
     * 
     */
    consoleSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_st>();
    
    /**
     * @brief @param mainlog is instantiated. It will contain all of the logging for the main program. Most of the logging will occure here. 
     * @brief @param sensorlog will contain all of the logging related to sensors, when they are registered, when they transfer data, etc.
     * @brief @param networklog will contain all of the network logging including MQTT message logs.
     */
    INST_LOGGER(mainlog,"main");
    INST_LOGGER(sensorslog,"sensors");
    INST_LOGGER(networklog,"network");

    /**
     * @brief This is the section for setting up configuration paramaters.
     * The first entry is the looked for file entry, the second is the default
     * paramater, and the third is 
     */
    
	cfg_opt_t opts[] = {
        CFG_STR("module","beehive",CFGF_NONE),
       	CFG_STR_LIST("serials","{28-012033dcea3b}",CFGF_NONE),
	    CFG_STR_LIST("sensors", "{temperature}", CFGF_NONE),
        CFG_STR_LIST("names","{insideTemp}",CFGF_NONE),
        CFG_INT("delays",60,CFGF_NONE), //In Seconds
        CFG_STR_LIST("inputTypes","{w1}",CFGF_NONE),
        /**
         * @brief Logging Setup in Cfg file. This allows for each of the following options. Default will be set to info. 
         */
        CFG_STR("mainlog","info",CFGF_NONE),
        CFG_STR("networklog","info",CFGF_NONE),
        CFG_STR("sensorslog","info",CFGF_NONE),
        CFG_END()
    };
    mainlog->debug("CFG File has been configured");

    /**
     * @brief cfg_init must be called to initialize the parsing of the cfg file.
     * if an error occurs, the cfg_parse will throw and error and kill the program.
     */
    cfg = cfg_init(opts, CFGF_NONE);
    //Checks for parsing error
	if(cfg_parse(cfg, cfgFile) == CFG_PARSE_ERROR) {
        mainlog->critical("Error parsing configuration file.");  
        return -1; 
    }

    /**
     * @brief Libconfuse parsing. The first section will deal specifically with the logging options
     * That have been setup in the configuration file. The second section will deal with the sensors section.
     * For now, sensors will only be configurable in the file. However, logging needs to be configurable from
     * the cmd line. cmd line will take precedence. If configured via cmd, @param cmdlog will be set to 1. The
     * sink will be console unless specified otherwise in cfg file.
     * 
     */
    if (cmdlog == 1){

        mainlog->set_level(spdlog::level::from_str(logLevel));
        networklog->set_level(spdlog::level::from_str(logLevel));
        sensorslog->set_level(spdlog::level::from_str(logLevel));
        
        mainlog->info("Log level set to {}",logLevel);
    }
    else{
        mainLogLevel = cfg_getstr(cfg, "mainlog");
	mainlog->set_level(spdlog::level::from_str(mainLogLevel));
        mainlog->debug("mainlog set to level: {}",mainLogLevel);

        networkLogLevel = cfg_getstr(cfg, "networklog");
	networklog->set_level(spdlog::level::from_str(networkLogLevel));
        networklog->debug("networklog set to level: {}",networkLogLevel);

        sensorsLogLevel = cfg_getstr(cfg, "sensorslog");
	sensorslog->set_level(spdlog::level::from_str(sensorsLogLevel));
        sensorslog->debug("sensorslog set to level: {}",sensorsLogLevel);
    }

    /**
     * @brief The next section run through the various cfg sections as created in the options section above.
     * They are each added to a list. The index of each list correspond to one sensor. @param module is the 
     * beehive, digestor, or etc. @param names is the custom name for the sensors. @param sensors is the vector
     * containing the type (temperature, humidity, etc), @param serials contains the serial address of sensor.
     * @param delays is the period of time between taking measurments. If these are not the same size the program
     *  will be killed.
     */
    //Pull the module name from the cfg File
    module = cfg_getstr(cfg,"module");
    
    //Pulls the Sensors from the cfg file ands puts them in sensor vector    
    for(i = 0;i<cfg_size(cfg,"names");++i){
       names.push_back(cfg_getnstr(cfg,"names",i));
       sensorslog->debug("Added {}",names.at(i));
    }

    //Pulls the Sensors from the cfg file ands puts them in sensor vector    
    for(i = 0;i<cfg_size(cfg,"sensors");++i){
        sensors.push_back(cfg_getnstr(cfg,"sensors",i));
        sensorslog->debug("Configured {} sensor as {}",names.at(i),sensors.at(i));
    }

    //Pulls the  from the cfg file ands puts them in sensor vector    
    for(i = 0;i<cfg_size(cfg,"serials");++i){
        serials.push_back(cfg_getnstr(cfg,"serials",i));
        sensorslog->debug("Added serial: {}",serials.at(i));
    }

    //Pulls the delay from the cfg file and puts them in types vector
    timeout_msecs = cfg_getint(cfg,"delays")*1000; //delays is in seconds so multiply by 1000

    //Pulls the input type for each sensor
    //Pulls the  from the cfg file ands puts them in sensor vector    
    for(i = 0;i<cfg_size(cfg,"inputTypes");++i){
        inputTypes.push_back(cfg_getnstr(cfg,"inputTypes",i));
        sensorslog->debug("Added inputType: {}",inputTypes.at(i));
    }

    /**
     * @brief This section checks for issues in the config file. If the vectors are different sizes then there is a missing
     * paramater. 
     */
    if(sensors.size() != serials.size()){ 
        sensorslog->critical("Configuration lists are not the same size!");
        return -1;
    }

    if (names.size() != serials.size()){ 
        sensorslog->critical("Configuration lists are not the same size!");
        return -1;
    }

    if (names.size() != inputTypes.size()){ 
        sensorslog->critical("Configuration lists are not the same size!");
        return -1;
    }

    /**
     * @brief This next check is for the inputTypes. Currently, the program will only accept w1 or i2c sensors.
     */
    for(i=0;i<inputTypes.size();i++){
        if(inputTypes.at(i)!="w1" && inputTypes.at(i)!="i2c"){
            mainlog->critical("%s is not supported.",inputTypes.at(i));
            return -1;
        }
    }

    /**
     * @brief Create an array named sensArray that will contain all of the objects from the sensor class. It's size is
     * based off of the number of inputs given in the configuration file.
     */
    sensor sensArray[sensors.size()];
    sensorslog->trace("Created sensory array of size " + to_string(sensors.size()));
    /**
     * @brief Loop through the sensArray and the config file entries and add the components to the respective objects.
     */
    for(i = 0; i < sensors.size();++i){
        sensArray[i].setName(names.at(i));
        sensorslog->trace("Set name for sensor " + to_string(i) + "to: " + names.at(i));
        sensArray[i].setModule(module);
        sensorslog->trace("Set module for sensor " + to_string(i) + "to: " + module);
        sensArray[i].setType(sensors.at(i));
        sensorslog->trace("Set type for sensor " + to_string(i) + "to: " + sensors.at(i));
        sensArray[i].setAddr(serials.at(i));
        sensorslog->trace("Set serial for sensor " + to_string(i) + "to: " + serials.at(i));
        sensArray[i].setDelay(timeout_msecs);
        sensorslog->trace("Set delay for sensor " + to_string(i) + "to: " + to_string(timeout_msecs) + "ms");
        sensArray[i].setInputType(inputTypes.at(i));
        sensorslog->trace("Set inputType for sensor" + to_string(i) + "to: " + inputTypes.at(i));
        sensorslog->debug("Addr:" + sensArray[i].getFullAddr());
    }

    /**
     * @brief Creation of the pollfd structure fds that is the size of how every many sensors are defined in the cfg file.
     */
    struct pollfd fds[sensors.size()];
    mainlog->trace("pollfd structure created with size: " + to_string(sensors.size()));

    //Define all of the pieces of the fds array for polliing
    //for(j=0;j<sensors.size();++j){
    //    //fds[j].fd = open("/sys/bus/w1/devices/28-012033dcea3b/temperature",O_RDONLY | O_NONBLOCK);
    //    fds[j].fd = open(sensArray[j].getFullAddr().c_str(), O_RDONLY | O_NONBLOCK);
    //    mainlog->debug("fds[" +to_string(j) + "] opened " + sensArray[j].getAddr());
    //    fds[j].events = POLLIN;
    //    fds[j].revents = 0;
    //}

    mainlog->trace("Poll Loop Starting");
    int testNum = 0;
    while(1){
	//Define all of the pieces of the fds array for polliing
	for(j=0;j<sensors.size();++j){
            //fds[j].fd = open("/sys/bus/w1/devices/28-012033dcea3b/temperature",O_RDONLY | O_NONBLOCK);
	    fds[j].fd = open(sensArray[j].getFullAddr().c_str(), O_RDONLY | O_NONBLOCK);
            mainlog->debug("fds[" +to_string(j) + "] opened " + sensArray[j].getAddr());
	    fds[j].events = POLLIN;
            fds[j].revents = 0;
    	}

	ret = poll(fds, sensors.size(), timeout_msecs);
	mainlog->debug("ret " + to_string(ret));
        if (ret > 0) {
            /* An event on one of the fds has occurred. */
            for (i=0; i<sensors.size(); ++i) {
                if (fds[i].revents & POLLIN) {
                    mainlog->trace("Read from Device: " + sensArray[i].getName());
                    readret = read(fds[i].fd,buf,7);
		    mainlog->debug("readret" + to_string(readret));
                    if(readret == -1){
                        mainlog->error("Read Error:",  strerror(errno));
                    }
                    else {
                        mainlog->debug("Temp read: " + to_string(atoi(buf)));
			testNum++;
			//This is the only time the data is sent to a payload
                        getData(sensArray[i],buf);
			//memset(buf, 0, sizeof buf); - I thought it might be an issue with the buffer not clearing, but the when this is set the value just shows 0
                        fds[i].events = 0;
                        fds[i].revents = 0;
		    }
                }
                else if (fds[i].revents & POLLHUP) {
                    mainlog->critical("Hangup");
                    return -1;
                }
                else if (fds[i].revents & POLLERR) {
                    mainlog->critical("Poll Error");
                    return -1;
                }
                //FUTURE FIXME: Handle all the errors including log messages (POLLNVAL)
            }
        }
        else if(ret == 0) {
            for(i=0; i<sensors.size(); ++i) {
                mainlog->warn("Timed Out!");
                fds[i].events = POLLIN;
                fds[i].revents = 0;
                read(fds[i].fd,buf,7);
                mainlog->warn("Timeout  read value:" + to_string(atoi(buf)));
		//testNum++;
            }
        }
	mainlog->trace("testNum: " + to_string(testNum));
    }
    //for(i=0;i<sensors.size();++i){
        //mqttGo(sensArray[i]);
    //    cout << sensArray[i].getName() << endl;
    //}
return 0;
}
