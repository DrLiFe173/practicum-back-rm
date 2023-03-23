#include "request_handler.h"
#include "response.h"

namespace http_handler {

    bool RequestHandler::IsInFileRootFolder(const std::filesystem::path& file_path) {
        auto static_folder_path = std::filesystem::current_path().append(fileRootFolder);
        std::string base_path = std::filesystem::weakly_canonical(static_folder_path).string();        

        return file_path.string().find(base_path) == 0;
    }

    VarResponse RequestHandler::ProceedFileResponse(const std::string requestTarget, unsigned http_version, bool isKeepAlive)
    {
        VarResponse response;
        auto file_path = GetFilePath(requestTarget);
        if (!IsInFileRootFolder(file_path.string())) {
            response = Response::MakeJSON(http::status::bad_request, ErrorCode::BAD_REQUEST, ErrorMessage::NO_ACCESS, ContentType::TEXT_PLAIN);
        } else if (IsFileExist(file_path)) {
            response = Response::MakeFileResponse(GetFilePath(requestTarget), http_version, isKeepAlive);
        }
        else
        {
            response = Response::MakeJSON(http::status::not_found, ErrorCode::FILE_NOT_FOUND, ErrorMessage::FILE_NOT_FOUND, ContentType::TEXT_PLAIN);
        }
        return response;
    }

    std::string RequestHandler::GetFileName(const std::filesystem::path& file_path) {        
        auto file_name = file_path.filename();
        return file_name.string();
    }

    std::filesystem::path RequestHandler::GetFilePath(const std::string& requestTarget) {

        auto base_path = std::filesystem::weakly_canonical(fileRootFolder);

        auto temp_path = base_path;
        temp_path.concat(requestTarget);
        auto res_path = std::filesystem::weakly_canonical(temp_path);
        
        if (requestTarget == "/") {
            res_path.append("index.html");
        }

        return res_path;
    }    

}  // namespace http_handler
