#pragma once

#include "pch.h"


/**
 * @brief Get the software version information.
 *
 * This function returns a string that contains the current version of the software, along
 * with the build date, time, and the system platform (Linux or Windows). It also includes
 * the version of Python being used in the environment.
 *
 * @return A string containing version, build date, system platform, and Python version.
 */
std::string version();
