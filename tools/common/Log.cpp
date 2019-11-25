//
// Created by zhou.yang on 2018/10/11.
//
#include "boost/algorithm/string.hpp"
//#include "boost/log/sinks/sync_frontend.hpp"
#include "boost/log/attributes/mutable_constant.hpp"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include "Exception.h"
#include "boost/filesystem.hpp"
#include "boost/random.hpp"
#include "StringUtil.h"
#include "boost/log/sinks/text_file_backend.hpp"

#include "Log.h"
using namespace std;
namespace logging = boost::log;

namespace keywords = boost::log::keywords;
namespace attrs    = boost::log::attributes;
namespace expr     = boost::log::expressions;
namespace sinks = boost::log::sinks;

namespace dipc{

    //typedef logging::sinks::synchronous_sink< logging::sinks::text_ostream_backend > text_sink;
    shared_ptr<logging::sources::severity_logger< logging::trivial::severity_level> > lg;
    //typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;

    logging::trivial::severity_level getLevel(const string &logLevel) {
        string lowerLevel = boost::algorithm::to_lower_copy(logLevel);

        if (lowerLevel == "trace") {
            return logging::trivial::trace;
        } else if (lowerLevel == "debug") {
            return logging::trivial::debug;
        } else if (lowerLevel == "info") {
            return logging::trivial::info;
        } else if (lowerLevel == "warning") {
            return logging::trivial::warning;
        } else if (lowerLevel == "error") {
            return logging::trivial::error;
        } else if (lowerLevel == "fatal") {
            return logging::trivial::fatal;
        }
        return logging::trivial::info;
    }

    void initLog(const string &logPath, const string &logLevel, const string& logName, bool verbose) {
        
        if(verbose){
            string logfile = logPath + "/" + logName;
            logging::core::get()->add_global_attribute("File", attrs::mutable_constant<std::string>(""));
            logging::core::get()->add_global_attribute("Line", attrs::mutable_constant<int>(0));
            logging::core::get()->set_filter(logging::trivial::severity>=getLevel(logLevel));


            logging::add_file_log (
                keywords::file_name = logfile,
                keywords::format = (
                        expr::stream
                                << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d_%H:%M:%S.%f")
                                << ": <" << boost::log::trivial::severity << "> "
                                << '['   << expr::attr<std::string>("File")
                                << ':' << expr::attr<int>("Line") << "] "
                                << expr::smessage
                )
            );
            logging::add_common_attributes();
            lg = make_shared<logging::sources::severity_logger< logging::trivial::severity_level>>();
           
        }else {
            std::cout  << "init log without log file " << std::endl;
            logging::core::get()->add_global_attribute("File", attrs::mutable_constant<std::string>(""));
            logging::core::get()->add_global_attribute("Line", attrs::mutable_constant<int>(0));
            //logging::core::get()->set
            //logging::add_common_attributes();
            logging::core::get()->set_filter(logging::trivial::severity>=getLevel(logLevel));
            lg = make_shared<logging::sources::severity_logger< logging::trivial::severity_level>>();
        }

    }


    // void init_term()
    // {

    //     typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
    //     boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
    //     // create sink to stdout
    //     boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
    //     sink->locked_backend()->add_stream(
    //         boost::shared_ptr<std::ostream>(&out, boost::empty_deleter()));

    //     // flush
    //     sink->locked_backend()->auto_flush(true);

    //     // format sink
    //     sink->set_formatter
    //     (
    //         /// TODO add your desired formatting
    //     );

    //     // filter
    //     // TODO add any filters

    //     // register sink
    //     bl::core::get()->add_sink(sink);
    // }



    void toInitLog(std::string project_name, bool verbose, std::string& logPath, std::string& logLevel, fs::path randomDir){
        if (randomDir.string() == ""){
            llvm::SmallString<64> res;
            llvm::sys::path::system_temp_directory(true, res);
            std::string randomPath = randomString(20)
                            .c_str() + project_name;
            randomDir = fs::path(std::string(res.c_str())) /
                        randomPath;
        }
            
        if (!fs::create_directories(randomDir))
        {
        throw Exception() << ErrStr("create dir failed:")
                            << ErrStr(strerror(errno));
        }
       
        if (verbose)
        {
            if (logPath.empty())
            {
                logPath = randomDir.string();
            }
            std::cout << project_name << " logDir    " << logPath << std::endl;
            std::string logName = project_name + ".log";
            initLog(logPath, logLevel, logName, verbose);
        } else {

            // if (!fs::create_directories(randomDir))
            // {
            // throw Exception() << ErrStr("create dir failed:")
            //                     << ErrStr(strerror(errno));
            // }
            std::cout  << "to init log without log file " << std::endl;
            initLog("", logLevel, "", verbose);
        }
    }
    

  // Logging macro
// #define LOG(level) BOOST_LOG_SEV(global_logger::get(), level)
// // Initializing global boost::log logger
// typedef boost::log::sources::severity_channel_logger_mt<
//     severity_level, std::string> global_logger_type;

// BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(global_logger, global_logger_type)
// {
//     return global_logger_type(boost::log::keywords::channel = "global_logger");
// }

// Initialize terminal logger
// void init_term(const string &logLevel)
// {
//    // typedef logging::sinks::synchronous_sink< logging::sinks::text_ostream_backend > text_sink;
//    // shared_ptr< text_sink > sink = make_shared< text_sink >();
//     // create sink to stdout
//     boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
//     sink->locked_backend()->add_stream(
//         boost::shared_ptr<std::ostream>(&out, boost::empty_deleter()));

//     // flush
//     sink->locked_backend()->auto_flush(true);

//     // format sink
//     sink->set_formatter
//     (
//         expr::stream
//             << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d_%H:%M:%S.%f")
//             << ": <" << boost::log::trivial::severity << "> "
//             << '['   << expr::attr<std::string>("File")
//             << ':' << expr::attr<int>("Line") << "] "
//             << expr::smessage
//     );

//     // filter
//     // TODO add any filters
//     sink->set_filter(
//         logging::trivial::severity>=getLevel(logLevel)
//     );
//     // register sink
//     logging::core::get()->add_sink(sink);
// }

// // Initialize logfile
// void init_logfile(const std::string& logfilename)
// {
//     //shared_ptr< text_sink > sink = make_shared< text_sink >();
//     // create sink to logfile
//     boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
//     sink->locked_backend()->add_stream(
//         boost::make_shared<std::ofstream>(logfilename.c_str()));

//     // flush
//     sink->locked_backend()->auto_flush(true);

//     // format sink
//     sink->set_formatter
//     (
//         /// TODO add your desired formatting
//     );

//     // filter
//     // TODO add any filters

//     // register sink
//     bl::core::get()->add_sink(sink);
// }

}
