#pragma once
#include <boost/asio.hpp>
#include <chrono>

namespace timer {
    namespace net = boost::asio;
    namespace sys = boost::system;
    using namespace std::chrono;
    using Timer = net::steady_timer;

    class Ticker : public std::enable_shared_from_this<Ticker> {
    public:
        using Strand = net::strand<net::io_context::executor_type>;
        using Handler = std::function<void(int64_t)>;

        Ticker(Strand strand, std::chrono::milliseconds period, Handler handler):strand_(strand), period_(period), handler_(handler){}

        void Start() {
            last_tick_ = steady_clock::now();
            /* Выполнить SchedulTick внутри strand_ */
            net::dispatch(strand_, [self = shared_from_this()] {
                self->ScheduleTick();
                });
        }
    private:
        void ScheduleTick() {
            /* выполнить OnTick через промежуток времени period_ */           
            timer_.expires_after(period_);
            timer_.async_wait([self = shared_from_this()](sys::error_code ec) {self->OnTick(ec);});
        }

        void OnTick(sys::error_code ec) {
            auto current_tick = steady_clock::now();
            auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(current_tick - last_tick_);
            handler_(delta.count());
            last_tick_ = current_tick;
            ScheduleTick();
        }

        Strand strand_;
        net::steady_timer timer_{ strand_ };
        std::chrono::milliseconds period_;
        Handler handler_;
        std::chrono::steady_clock::time_point last_tick_;
    };
} // namespace timer
