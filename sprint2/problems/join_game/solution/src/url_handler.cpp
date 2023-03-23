#include "url_handler.h"

namespace http_handler {
	std::string UrlHandler::UrlDecode(const std::string& input) {
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
}