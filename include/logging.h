#pragma once

#include <boost/log/expressions.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>

// Narrow-char thread-safe logger.
typedef boost::log::sources::severity_logger_mt<
    boost::log::trivial::severity_level>
    logger_t;

// declares a global logger with a custom initialization
BOOST_LOG_GLOBAL_LOGGER(my_logger, logger_t)

#define TRACE boost::log::trivial::trace
#define DEBUG boost::log::trivial::debug
#define INFO boost::log::trivial::info
#define WARN boost::log::trivial::warning
#define ERROR boost::log::trivial::error
#define FATAL boost::log::trivial::fatal
