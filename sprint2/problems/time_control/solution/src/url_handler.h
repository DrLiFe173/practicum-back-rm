#pragma once
#include <string>

namespace http_handler {
	class UrlHandler {
	public:
		UrlHandler() = delete;
		static std::string UrlDecode(const std::string& input);
	};
} // namespace http_handler