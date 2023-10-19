#pragma once

#include <iomanip>
#include <iostream>

#define LogTrace(str) std::clog << "TRACE\t" << std::left << std::setw(30) << __FUNCTION__ << " : " << str << std::endl;
#define LogDebug(str) std::clog << "DEBUG\t" << std::left << std::setw(30) << __FUNCTION__ << " : " << str << std::endl;
#define LogInfo(str) std::clog << "INFO\t" << std::left << std::setw(30) << __FUNCTION__ << " : " << str << std::endl;
#define LogWarn(str) std::clog << "WARN\t" << std::left << std::setw(30) << __FUNCTION__ << " : " << str << std::endl;
#define LogError(str) std::clog << "ERROR\t" << std::left << std::setw(30) << __FUNCTION__ << " : " << str << std::endl;