#include "sdk.h"
//
#include <boost/program_options.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_context.hpp>
#include <iostream>
#include <thread>
#include <chrono>

#include "logger.h"
#include "json_loader.h"
#include "request_handler.h"
#include "magic_defs.h"
#include "ticker.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;

namespace {

    struct Args {
        std::string config_root;
        std::string static_files_root;
        int64_t ticks;
        bool randomize_spawn;
    };

    [[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* const argv[]) {
        namespace po = boost::program_options;

        po::options_description desc{ "All options"s };
        Args args;
        desc.add_options()           
            ("help,h", "produce help message")  
            ("tick-period,t", po::value<int64_t>(&args.ticks)->value_name("milliseconds"s), "set tick period")
            ("config-file,c", po::value(&args.config_root)->value_name("file"s), "set config file path")
            ("www-root,w", po::value(&args.static_files_root)->value_name("dir"s), "set static files root")
            ("randomize-spawn-points", "spawn dogs at random positions");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.contains("help"s)) {
            std::cout << desc;
            return std::nullopt;
        }

        if (!vm.contains("config-file"s)) {
            throw std::runtime_error(ServerMessage::ERROR_ARGS.data());
        }

        if (!vm.contains("www-root"s)) {
            throw std::runtime_error(ServerMessage::ERROR_ARGS.data());
        }

        if (!vm.contains("tick-period"s)) {
            args.ticks = 0;
        }

        if (vm.contains("randomize-spawn-points"s)) {
            args.randomize_spawn = true;
        }
        else {
            args.randomize_spawn = false;
        }
        return args;
    };

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

}  // namespace


int main(int argc, const char* argv[]) {
    try {
        if (auto args = ParseCommandLine(argc, argv)) {      
            Logger::SetupLogger();

            // 1. Загружаем карту из файла и построить модель игры
            model::Game game;
            json_loader::LoadGame(args->config_root, game);
            game.SetDefaultTicks(args->ticks);
            game.SetRandomSpawnFlag(args->randomize_spawn);

            // 2. Инициализируем io_context
            const unsigned num_threads = std::thread::hardware_concurrency();
            net::io_context ioc(num_threads);

            // 3. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM
            net::signal_set signals(ioc, SIGINT, SIGTERM);
            signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
                if (!ec) {
                    ioc.stop();
                }
                });

            // 4. Создаём обработчик HTTP-запросов и связываем его с моделью игры, std::make_shared т.к. используем strand в RequestHandler
            net::strand<net::io_context::executor_type> strand{ net::make_strand(ioc) };
            auto handler = std::make_shared<http_handler::RequestHandler>(game, strand);
            auto files_path = args->static_files_root;
            handler->SetRootFolderPath(files_path.c_str());


            if (game.IsGameTimerEnabled()) {
                std::chrono::milliseconds period = std::chrono::milliseconds(game.GetDefaultTicks());
                auto timer = std::make_shared<timer::Ticker>(strand, period, [&](int64_t ticks) { game.UpdateGameSessions(ticks); });
                timer->Start();
            }

            // 5. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
            const auto address = net::ip::make_address(ServerParam::ADDR);
            constexpr net::ip::port_type port = ServerParam::PORT;
            http_server::ServeHttp(ioc, { address, port }, [&handler](auto&& req, auto&& send) {
                handler->operator()(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
                });

            // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы;
            std::string server_address = address.to_string();
            Logger::LogServerStart(port, server_address);

            // 6. Запускаем обработку асинхронных операций
            RunWorkers(std::max(1u, num_threads), [&ioc] {
                ioc.run();
                });
        }        
    } catch (const std::exception& ex) {
        std::string exception{ ex.what() };
        Logger::LogServerStop(EXIT_FAILURE, exception);
        return EXIT_FAILURE;
    }
    Logger::LogServerStop(0);
}
