#pragma once

#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>

using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {
    auto GetTime() const {
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        return std::put_time(std::localtime(&t_c), "%F %T");
    }

    // Для имени файла возьмите дату с форматом "%Y_%m_%d"
    std::string GetFileTimeStamp() const {
        auto t_c = std::chrono::system_clock::to_time_t(GetTime());
        auto date_formatted = std::put_time(std::localtime(&t_c), "%Y_%m_%d");
        std::ostringstream oss;
        oss << date_formatted;
        return oss.str();
    }

    std::string GetStringTimeStamp() const {
        auto date_formatted = GetTimeStamp();
        std::ostringstream oss;
        oss << date_formatted;
        return oss.str();
    }

    Logger() = default;
    Logger(const Logger&) = delete;

public:
    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    template<typename T, typename... Ts>
    std::string R(const T& value) {  
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    // Выведите в поток все аргументы.
    template<typename T, typename... Ts>
    std::string R(const T& value, const Ts&... args) {
        std::string log_string = R(value);
        log_string += R(args...);
        return log_string;
    }

    template<typename... Ts>
    void Log(const Ts&... args) {
        std::string log_string = R(args...);
        std::string file_name = log_path;
        std::lock_guard lg(m_);
        file_name.append("sample_log_"s);
        file_name.append(GetFileTimeStamp());
        file_name.append(".log"s);
        log_file_.open(file_name, std::ios::app);
        log_file_ << GetStringTimeStamp() << ": "s << log_string << '\n';
        log_file_.close();
    }

    // Установите manual_ts_. Учтите, что эта операция может выполняться
    // параллельно с выводом в поток, вам нужно предусмотреть 
    // синхронизацию.
    void SetTimestamp(std::chrono::system_clock::time_point ts) {
        std::lock_guard lg(m_);
        manual_ts_ = ts;
    }

private:
    std::optional<std::chrono::system_clock::time_point> manual_ts_;
    std::ofstream log_file_;
    std::string log_path = "/var/log/"; 
    std::mutex m_;   
};