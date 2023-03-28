#pragma once
#include <boost/json.hpp>
class Logger {
    Logger() = default;
    Logger(const Logger&) = delete;
 
public:
    static void SetupLogger();

    static void Log(boost::json::object& json, std::string& message);

    static void LogRequest(std::string& ip, std::string& uri, std::string& method);
    static void LogResponse(std::string& ip, long long time, int code, std::string& content_type);
    static void LogServerStop(int code, std::string& exception);
    static void LogServerStop(int code);
    static void LogServerStart(int code, std::string& exception);
    static void LogWebError(int code, std::string& exception, std::string& where);
    static void LogServerError(std::string& exception, std::string& where);
    static void LogServerMessage(std::string& message, std::string& where);
};
        
