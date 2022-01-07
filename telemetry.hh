/**
 * @file telemetry.hh
 * @author Daniel Ballif (ballifdaniel@gmail.com)
 * @brief 
 * 
 */


#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <string>

//FIXME: Comment these
//FIXME: Look at apsiproxy version.hh and githash (version-githash)
#define GETOPT_OPTSTRING_PLATFORM ""
#define GETOPT_OPTSTRING_COMMON "vhf:d:"
#define GETOPT_OPTSTRING GETOPT_OPTSTRING_COMMON GETOPT_OPTSTRING_PLATFORM

std::string version = "0.0.0";

#endif //TELEMETRY_H


