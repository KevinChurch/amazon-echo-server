#include "logging.h"

namespace attrs   = boost::log::attributes;
namespace expr    = boost::log::expressions;
namespace logging = boost::log;

namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

//Defines a global logger initialization routine
BOOST_LOG_GLOBAL_LOGGER_INIT(my_logger, logger_t)
{
    logger_t lg;
    logging::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");
    logging::add_common_attributes();

    logging::add_file_log(
      keywords::file_name = "server_log_%Y-%m-%d_%H:%M:%S_%N.log",
      keywords::rotation_size = 10 * 1024 * 1024,   // rotate when the file size reaches 10 MB
      keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
      keywords::auto_flush = true,
      keywords::format = "[%TimeStamp%] [Thread %ThreadID%] [%Severity%]: %Message%"
    );

    logging::add_console_log(
      std::cout,
      keywords::format = "[%TimeStamp%] [Thread %ThreadID%] [%Severity%]: %Message%"
    );

    return lg;
}
