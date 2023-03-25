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
#include "magic_defs.h"
 
using namespace std::literals;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;
 
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(additional_data, "AdditionalData", boost::json::object)
 
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
    void Logger::Log(boost::json::object& json, std::string& message) {
        BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, json) << message;
    }

    void Logger::LogRequest(std::string& ip, std::string& uri, std::string& method) {
        boost::json::object json;
        json[LoggerDefs::IP] = ip;
        json[LoggerDefs::URI] = uri;
        json[LoggerDefs::METHOD] = method;
        std::string message = LoggerDefs::REQ_RECIEVED;
        Log(json, message);
    }

    void Logger::LogResponse(std::string& ip, long long time, int code, std::string& content_type) {
        boost::json::object json;
        json[LoggerDefs::IP] = ip;
        json[LoggerDefs::RESP_TIME] = time;
        json[LoggerDefs::CODE] = code;
        json[LoggerDefs::CONTENT_TYPE] = content_type;
        std::string message = LoggerDefs::RESP_SENT;
        Log(json, message);
    }

    void Logger::LogServerStop(int code, std::string& exception) {
        boost::json::object json;
        json[LoggerDefs::CODE] = code;
        json[LoggerDefs::EXCEPTION] = exception;
        std::string message = ServerMessage::EXIT.data();
        Log(json, message);
    }

    void Logger::LogServerStop(int code) {
        boost::json::object json;
        json[LoggerDefs::CODE] = code;
        std::string message = ServerMessage::EXIT.data();
        Log(json, message);
    }

    void Logger::LogServerStart(int port, std::string& address) {
        boost::json::object json;
        json[LoggerDefs::PORT] = port;
        json[LoggerDefs::ADDRESS] = address;
        std::string message = ServerMessage::START.data();
        Log(json, message);        
    }

    void Logger::LogWebError(int code, std::string& exception, std::string& where) {
        boost::json::object json;
        json[LoggerDefs::CODE] = code;
        json[LoggerDefs::EXCEPTION] = exception;
        json[LoggerDefs::WHERE] = where;
        std::string message = LoggerDefs::ERR;
        Log(json, message);
    }

    void Logger::LogServerError(std::string& exception, std::string& where) {
        boost::json::object json;
        json[LoggerDefs::EXCEPTION] = exception;
        json[LoggerDefs::WHERE] = where;
        std::string message = LoggerDefs::ERR;
        Log(json, message);
    }

    void Logger::LogServerMessage(std::string& info, std::string& where) {
        boost::json::object json;
        json[LoggerDefs::WHERE] = where;
        std::string message = LoggerDefs::INFO;
        Log(json, message);
    }