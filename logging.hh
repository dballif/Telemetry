/**
 * @file logging.hh
 * @author Daniel Ballif (ballifdaniel@gmail.com)
 * @brief Logging for telemetry.cc
 * 
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#define DECL_LOGGER(X) extern decltype(spdlog::get("")) X;
#define INST_LOGGER(LOGGER, LOGGER_NAME) LOGGER = std::make_shared<spdlog::logger>(LOGGER_NAME, consoleSink);
#define DEF_LOGGER(X) decltype(spdlog::get("")) X;

/**
 * @brief This is the defining of the various logs used within the program. If any more are to be added
 * They need to be added here, in logging.hh and a little lower with the INST_LOGGER things later.
 * @param mainLogLevel, @param networkLogLevel, and @param sensorsLogLevel are the strings needed for 
 * the configurtion file parsing. The shared_ptr is also important to setup @param consoleSink.
 * 
 */
DEF_LOGGER(mainlog);
DEF_LOGGER(networklog);
DEF_LOGGER(sensorslog);

DECL_LOGGER(mainlog);
DECL_LOGGER(networklog);
DECL_LOGGER(sensorslog);

#endif //LOGGING_H
