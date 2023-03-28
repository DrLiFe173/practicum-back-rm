#include "http_server.h"
#include <iostream>
#include "magic_defs.h"

namespace http_server {

// Разместите здесь реализацию http-сервера, взяв её из задания по разработке асинхронного сервера
	void ReportError(beast::error_code ec, std::string_view what) {
		std::string message = ec.message();
		std::string place = what.data();
		Logger::LogWebError(ec.value(), message, place);
	};

	void SessionBase::Run() {
		// Вызываем метод Read, используя executor объекта stream_.
		// Таким образом вся работа со stream_ будет выполняться, используя его executor
		net::dispatch(stream_.get_executor(),
			beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
	};

	void  SessionBase::OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written) {
		if (ec) {
			return ReportError(ec, ServerAction::WRITE);
		}

		if (close) {
			// Семантика ответа требует закрыть соединение
			return Close();
		}


		// Считываем следующий запрос
		Read();
	};

	void SessionBase::Read() {		
		// Очищаем запрос от прежнего значения (метод Read может быть вызван несколько раз)
		request_ = {};
		stream_.expires_after(30s);
		// Считываем request_ из stream_, используя buffer_ для хранения считанных данных
		start_ts_ = std::chrono::system_clock::now();		

		http::async_read(stream_, buffer_, request_,
			// По окончании операции будет вызван метод OnRead
			beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
	};

	void SessionBase::OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read) {
		if (ec == http::error::end_of_stream) {
			// Нормальная ситуация - клиент закрыл соединение
			std::string message = ec.message();
			std::string place = ServerAction::READ.data();
			Logger::LogWebError(ec.value(), message, place);
			return Close();
		}
		if (ec) {
			return ReportError(ec, ServerAction::READ);
		}
		HandleRequest(std::move(request_));
	};

	void SessionBase::Close() {
		beast::error_code ec;
		stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
	};

}  // namespace http_server
