#include "url_handler.h"

namespace http_handler {
	std::string UrlHandler::UrlDecode(const std::string& input) {
        char hex[3];                                // ��������� ����� ��� �������� %XX
        int code;                                   // ��������������� ���
        std::string res;                            // ������������ ������

        for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
            if (*iter == '%') {                     // ���� ������ "%", �� ������� ������� URL ������ 
                ++iter;
                if (iter != input.cend()) {         // �������� �� ����� �� ����� �������
                    hex[0] = *iter;
                    ++iter;
                    if (iter != input.cend()) {     // �������� �� ����� �� ����� �������
                        hex[1] = *iter;
                        hex[2] = 0;
                        sscanf(hex, "%X", &code);
                        res += (char)code;
                    }
                    else {                          // ������ ����������� ���������� ������ %X
                        --iter;
                        res += *(--iter);
                        res += hex[0];
                        break;
                    }
                }
                else {                              // ������ ����������� ���������� ������ %
                    res += *(--iter);
                    break;
                }
            }
            else if (*iter == '+') {                // ���� ������ "+", �� �������� ��� �� " "
                res += " ";
            }
            else {
                res += *iter;
            }
        }
        return res;
	}
}