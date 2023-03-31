#pragma once
#include "http_server.h"
#include "model.h"
#include <iostream>
#include <boost/json.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_context.hpp>
#include <filesystem>
#include <fstream>
#include <variant>
#include "logger.h"
#include "magic_defs.h"
#include "api_handler.h"
#include "url_handler.h"

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
namespace fs = std::filesystem;
namespace net = boost::asio;
using namespace std::literals;

// Запрос, тело которого представлено в виде строки
using StringRequest = http::request<http::string_body>;
// Ответ, тело которого представлено в виде строки
using StringResponse = http::response<http::string_body>;

using FileResponse = http::response<http::file_body>;

using VarResponse = std::variant<StringResponse, FileResponse>;

class RequestHandler : public std::enable_shared_from_this<RequestHandler> {
public:
    explicit RequestHandler(model::Game& game, net::strand<net::io_context::executor_type>& strand)
        : game_{game},
        apiHandler{game},
        strand_(strand){
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    std::filesystem::path GetFilePath(const std::string& requestTarget);

    VarResponse ProceedFileResponse(const std::string requestTarget, unsigned http_version, bool isKeepAlive);    

    bool IsInFileRootFolder(const std::filesystem::path& file_path);

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        // Обработать запрос request и отправить ответ, используя send

        std::string target_text(req.target());
        std::string decoded_request = UrlHandler::UrlDecode(target_text);

        VarResponse response;
        if (apiHandler.IsApiRequest(decoded_request.c_str())) {
            return net::dispatch(strand_, [self = shared_from_this(), send, req] {
            assert(self->strand_.running_in_this_thread());
            StringResponse resp = self->apiHandler.ProceedApiRequest(req);
            send(resp);
            });
        }
        else {        
            response = ProceedFileResponse(decoded_request, req.version(), req.keep_alive());
            if (std::holds_alternative<FileResponse>(response)) {
                send(std::get<FileResponse>(response));
            }
            else {
                send(std::get<StringResponse>(response));
            }
        }
    }

    bool IsFileExist(const std::filesystem::path& path) {
        return std::filesystem::exists(path);
    }

    void SetRootFolderPath(const char* path) {
        fileRootFolder = path;
    }

private:
    model::Game& game_;
    ApiRequestHandler apiHandler;
    std::string fileRootFolder;
    net::strand<net::io_context::executor_type>& strand_;
};

}  // namespace http_handler
