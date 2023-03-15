#include "request_handler.h"

namespace http_handler {

    bool RequestHandler::IsApiRequest(const std::string requestTarget) {
        return requestTarget.substr(0, 5) == "/api/"sv ? true : false;
    }

    bool RequestHandler::IsMapRequest(const std::string requestTarget) {
        return requestTarget.find_first_of("/maps"sv) <= requestTarget.size() ? true : false;
    }

    bool RequestHandler::IsApiVersionCorrect(const std::string requestTarget) {
        return requestTarget.substr(4, 4) == "/v1/"sv ? true : false;
    }

    bool RequestHandler::IsInFileRootFolder(const std::string& file_path) {
        auto static_folder_path = std::filesystem::current_path().append(fileRootFolder);
        std::string base_path = std::filesystem::weakly_canonical(static_folder_path).string();

        return file_path.find(base_path) == 0;
    }

    std::string_view RequestHandler::GetGontentType(const std::string& file_name) {
        auto find_res = file_name.rfind(".");
        std::string file_extension = file_name.substr(find_res);

        if (boost::iequals(file_extension, ContentType::HTM) || boost::iequals(file_extension, ContentType::HTML)) {
            return ContentType::TEXT_HTML;
        }
        else if (boost::iequals(file_extension, ContentType::CSS)) {
            return ContentType::TEXT_CSS;
        }
        else if (boost::iequals(file_extension, ContentType::TXT)) {
            return ContentType::TEXT_PLAIN;
        }
        else if (boost::iequals(file_extension, ContentType::JAVASCRIPT)) {
            return ContentType::TEXT_JAVASCRIPT;
        }
        else if (boost::iequals(file_extension, ContentType::JSON)) {
            return ContentType::APP_JSON;
        }
        else if (boost::iequals(file_extension, ContentType::XML)) {
            return ContentType::APP_XML;
        }
        else if (boost::iequals(file_extension, ContentType::PNG)) {
            return ContentType::IMAGE_PNG;
        }
        else if (boost::iequals(file_extension, ContentType::JPE) || boost::iequals(file_extension, ContentType::JPEG) || boost::iequals(file_extension, ContentType::JPG)) {
            return ContentType::IMAGE_JPG;
        }
        else if (boost::iequals(file_extension, ContentType::GIF)) {
            return ContentType::IMAGE_GIF;
        }
        else if (boost::iequals(file_extension, ContentType::BMP)) {
            return ContentType::IMAGE_BMP;
        }
        else if (boost::iequals(file_extension, ContentType::ICO)) {
            return ContentType::IMAGE_ICO;
        }
        else if (boost::iequals(file_extension, ContentType::TIF) || boost::iequals(file_extension, ContentType::TIFF)) {
            return ContentType::IMAGE_TIF;
        }
        else if (boost::iequals(file_extension, ContentType::SVG) || boost::iequals(file_extension, ContentType::SVGZ)) {
            return ContentType::IMAGE_SVG;
        }
        else if (boost::iequals(file_extension, ContentType::MP3)) {
            return ContentType::AUDIO_MP3;
        }
        else {
            return ContentType::APP_BYTE;
        }
    }

    StringResponse RequestHandler::MakeStringResponce(const std::string requestTarget, unsigned http_version, bool isKeepAlive) {

        http::status status;
        std::string body;
        StringResponse response;

        if (IsApiRequest(requestTarget)) {
            if (IsApiVersionCorrect(requestTarget)) {
                if (IsMapRequest(requestTarget)) {
                    std::string mapId = GetMapIdFromRequestTarget(requestTarget);
                    response.set(http::field::content_type, ContentType::APP_JSON);
                    if (mapId == "maps"sv) {
                        status = http::status::ok;
                        AddAllMapsInfo(body);
                    }
                    else {
                        model::Map::Id id{ mapId };
                        auto requested_map = game_.FindMap(id);
                        if (requested_map != NULL) {
                            status = http::status::ok;
                            AddMapInfo(body, mapId);
                        }
                        else {
                            status = http::status::not_found;
                            CreateErrorResponce(body, "mapNotFound", "Map not found");
                        }
                    }
                }
            }
            else {
                status = http::status::bad_request;
                CreateErrorResponce(body, "badRequest", "Bad request");
                response.set(http::field::content_type, ContentType::APP_JSON);
            }
        }
        else {
            std::string file_path = GetFilePath(requestTarget);
            if (IsFileExist(file_path)) {
                status = http::status::ok;
            }
            else if (!IsInFileRootFolder(file_path)) {
                status = http::status::bad_request;
                CreateErrorResponce(body, "badRequest", "No access");
                response.set(http::field::content_type, ContentType::TEXT_PLAIN);
            }
            else 
            {
                status = http::status::not_found;
                CreateErrorResponce(body, "fileNotFound", "File not found");
                response.set(http::field::content_type, ContentType::TEXT_PLAIN);
            } 
        }
        response.result(status);
        response.body() = body;
        response.content_length(body.size());
        return response;
    }

    std::string RequestHandler::GetFileName(const std::string& file_path) {
        fs::path path{ file_path };
        auto file_name = path.filename();

        return file_name.string();
    }

    std::string RequestHandler::GetMapIdFromRequestTarget(const std::string requestTarget) {
        auto find_res = requestTarget.rfind("/");
        std::string mapId = requestTarget.substr(++find_res);
        return mapId;
    }

    void RequestHandler::CreateErrorResponce(std::string& body, const std::string& code, const std::string& message) {
        json::object obj;
        obj["code"] = code;
        obj["message"] = message;
        body = json::serialize(obj);
    }

    void RequestHandler::AddAllMapsInfo(std::string& body) {
        auto maps = game_.GetMaps();
        json::array jsonArr;
        for (auto map : maps) {
            json::object mapObj;
            mapObj["id"] = *map.GetId();
            mapObj["name"] = map.GetName();
            jsonArr.push_back(mapObj);
        }
        body = json::serialize(jsonArr);
    }

    void RequestHandler::AddRoadsInfo(const model::Map* map, json::object& object) {
        json::array jsonArr;
        for (auto road : map->GetRoads()) {
            model::Point start = road.GetStart();
            json::object tempObj;
            tempObj["x0"] = start.x;
            tempObj["y0"] = start.y;
            if (road.IsHorizontal()) {
                tempObj["x1"] = road.GetEnd().x;
            }
            else {
                tempObj["y1"] = road.GetEnd().y;
            }
            jsonArr.push_back(tempObj);
        }
        object["roads"] = jsonArr;
    }

    void RequestHandler::AddBuildingsInfo(const model::Map* map, json::object& object) {
        json::array jsonArr;
        for (auto building : map->GetBuildings()) {
            model::Rectangle bound = building.GetBounds();
            json::object tempObj;
            tempObj["x"] = bound.position.x;
            tempObj["y"] = bound.position.y;
            tempObj["w"] = bound.size.width;
            tempObj["h"] = bound.size.height;
            jsonArr.push_back(tempObj);
        }
        object["buildings"] = jsonArr;
    }

    void RequestHandler::AddOfficesInfo(const model::Map* map, json::object& object) {
        json::array jsonArr;
        for (auto office : map->GetOffices()) {
            json::object tempObj;
            tempObj["id"] = *office.GetId();
            tempObj["x"] = office.GetPosition().x;
            tempObj["y"] = office.GetPosition().y;
            tempObj["offsetX"] = office.GetOffset().dx;
            tempObj["offsetY"] = office.GetOffset().dy;
            jsonArr.push_back(tempObj);
        }
        object["offices"] = jsonArr;
    }

    void RequestHandler::AddMapInfo(std::string& body, std::string& mapId) {
        model::Map::Id id{ mapId };
        auto requested_map = game_.FindMap(id);
        if (requested_map != NULL) {
            json::object obj;
            obj["id"] = *requested_map->GetId();
            obj["name"] = requested_map->GetName();

            AddRoadsInfo(requested_map, obj);
            AddBuildingsInfo(requested_map, obj);
            AddOfficesInfo(requested_map, obj);

            body = json::serialize(obj);
        }
    }

    std::string RequestHandler::UrlDecode(const std::string& input) {
        char hex[3];                                // временный буфер для хранения %XX
        int code;                                   // преобразованный код
        std::string res;                            // возвращаемая строка

        for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
            if (*iter == '%') {                     // если символ "%", то пробуем извлечь URL символ 
                ++iter;
                if (iter != input.cend()) {         // проверка на выход за конец массива
                    hex[0] = *iter;                    
                    ++iter;
                    if (iter != input.cend()) {     // проверка на выход за конец массива
                        hex[1] = *iter;
                        hex[2] = 0;
                        sscanf(hex, "%X", &code);
                        res += (char)code;
                    } 
                    else {                          // строка закончилась дописываем символ %X
                        --iter;
                        res += *(--iter);
                        res += hex[0];
                        break;
                    }                      
                }
                else {                              // строка закончилась дописываем символ %
                    res += *(--iter);
                    break;
                }                                    
            }
            else if (*iter == '+') {                // если символ "+", то заменяем его на " "
                res += " ";
            }
            else {
                res += *iter;
            }            
        }
        return res;
    }

    std::string RequestHandler::GetFilePath(const std::string& requestTarget) {

        auto base_path = std::filesystem::weakly_canonical(fileRootFolder);

        auto temp_path = base_path;
        temp_path.concat(requestTarget);
        auto res_path = std::filesystem::weakly_canonical(temp_path);
        
        if (requestTarget == "/") {
            res_path.append("index.html");
        }

        return res_path.string();
    }

    FileResponse RequestHandler::MakeFileResponce(const std::string& file_path, unsigned http_version, bool isKeepAlive) {

        http::status status = (http::status::ok);
        FileResponse response(status, http_version);

        std::string file_name = GetFileName(file_path);

        response.insert(http::field::content_type, GetGontentType(file_name));

        http::file_body::value_type file;
        if (boost::system::error_code ec; file.open(file_path.c_str(), beast::file_mode::read, ec), ec) {
            std::cout << "Failed to open file "sv << file_path << std::endl;
        }
        response.body() = std::move(file);
        response.prepare_payload();

        return response;
    }

}  // namespace http_handler
