#define BOOST_BEAST_USE_STD_STRING_VIEW
 
#include "logger.h"
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/utility/setup/console.hpp> 
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/date_time.hpp>
#include <boost/log/utility/manipulators/add_value.hpp> 
 
using namespace std::literals;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;
 
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(additional_data, "AdditionalData", boost::json::value)
 
    void Logger::SetupLogger()
    {
        logging::add_common_attributes();
        logging::add_console_log(
            std::cout,
            boost::log::keywords::format = [](logging::record_view const& rec, logging::formatting_ostream& strm) 
            {
                auto ts = *rec[timestamp];
                strm << "{\"timestamp\":\"" << to_iso_extended_string(ts) << "\",";
                strm << "\"data\":" << rec[additional_data] << ",";
                strm << "\"message\":\"" << rec[expr::smessage] << "\"}"; 
            },
            keywords::auto_flush = true);
    }

    void Logger::LogRequest(std::string& ip, std::string& uri, std::string& method) {
        boost::json::object json;
        json["ip"] = ip;
        json["URI"] = uri;
        json["method"] = method;
        boost::json::value json_value = json;
        std::string message = "request received"s;
        BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, json_value) << message;
    }

    void Logger::LogResponse(std::string& ip, long long time, int code, std::string& content_type) {
        boost::json::object json;
        json["ip"] = ip;
        json["response_time"] = time;
        json["code"] = code;
        json["content_type"] = content_type;
        boost::json::value json_value = json;
        std::string message = "response sent"s;
        BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, json_value) << message;
    }

    void Logger::LogServerStop(int code, std::string& exception) {
        boost::json::object json;
        json["code"] = code;
        json["exception"] = exception;        
        boost::json::value json_value = json;
        std::string message = "server exited"s;
        BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, json) << message;
    }

    void Logger::LogServerStop(int code) {
        boost::json::object json;
        json["code"] = code;
        boost::json::value json_value = json;
        std::string message = "server exited"s;
        BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, json) << message;
    }

    void Logger::LogServerStart(int port, std::string& address) {
        boost::json::object json;
        json["port"] = port;
        json["address"] = address;
        boost::json::value json_value = json;
        std::string message = "server started"s;
        BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, json) << message;
    }

    void Logger::LogWebError(int code, std::string& exception, std::string& where) {
        boost::json::object json;
        json["code"] = code;
        json["exception"] = exception;
        json["where"] = where;
        boost::json::value json_value = json;
        std::string message = "error"s;
        BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, json) << message;
    }