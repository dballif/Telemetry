/**
 * \file version.hh
 *
 * The telemetry version file based off of the apsiproxy version.
 * I'm assuming that the makefile or building or somethign will increment these
 */

#ifndef VERSION_H_
#define VERSION_H_

/**
 * @brief cpputil.hh is important to include for preprocessing tools.
 *
 */
#include "cpputil.hh"

/*!\def TELEMETRY_MAJOR
 * \brief define the current major version of the program
 */
#define TELEMETRY_MAJOR 0

/*!\def TELEMETRY_MINOR
 * \brief define the current minor version of the program
 */
#define TELEMETRY_MINOR 6

/*!\def TELEMETRY_PATCH
 * \brief define the current patch level of the program
 */
#define TELEMETRY_PATCH 0

/*!\def TELEMETRY_BUILD
 * \brief define the current build number of the program
 */
//#define TELEMETRY_BUILD GITREV  //Will eventually define GITREV in make file to grab the git hash

/*!\def TELEMETRY_VERSION
 * \brief define the current version of the program
 */
#define TELEMETRY_VERSION PP_STR(TELEMETRY_MAJOR) "." PP_STR(TELEMETRY_MINOR) "." PP_STR(TELEMETRY_PATCH)

/*!\def TELEMETRY_FULLVERSION
 * \brief The full version is the version + the build ID.
 */
//#define TELEMETRY_FULLVERSION TELEMETRY_VERSION "-" TELEMETRY_BUILD

#endif
