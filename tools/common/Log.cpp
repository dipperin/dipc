//
// Created by zhou.yang on 2018/10/11.
//
#include "boost/algorithm/string.hpp"
#include "boost/log/attributes/mutable_constant.hpp"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include "Exception.h"
#include "boost/filesystem.hpp"
#include "boost/random.hpp"
#include "StringUtil.h"

#include "Log.h"
using namespace std;
namespace logging = boost::log;

namespace keywords = boost::log::keywords;
namespace attrs    = boost::log::attributes;
namespace expr     = boost::log::expressions;
namespace dipc{

    shared_ptr<logging::sources::severity_logger< logging::trivial::severity_level> > lg;

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

    void initLog(const string &logPath, const string &logLevel, string& logName) {
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
    }


    void toInitLog(std::string project_name, bool verbose, std::string& logPath, std::string& logLevel, fs::path randomDir){
        if (randomDir.string() == ""){
            llvm::SmallString<64> res;
            llvm::sys::path::system_temp_directory(true, res);
            std::string randomPath = randomString(20)
                            .c_str() + project_name;
            randomDir = fs::path(std::string(res.c_str())) /
                        randomPath;
        }
        
        //std::cout << "randomDir.string()  " << " logDir    " << randomDir.string() << std::endl;
        //std::cout << "existLogPath logDir    " << existLogPath << std::endl;
        // if(randomDir.string() == existLogPath){
        //     return;
        // }
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
            initLog(logPath, logLevel, logName);
        }
    }
    
}
