#include <string.h>
#include <ctype.h>
#include "utils.h"

std::string format_vec(const uint8_t *vec, size_t len, const char *format) {
    std::string str{};
    char buf[16];

    if (!format)
        format = "%02hhx";
    for (size_t n = 0; n < len; n++) {
        if (!str.empty())
            str.append(" ");
        std::snprintf(buf, sizeof(buf), format, vec[n]);
        str.append(buf);
    }
    return str;
}

std::string format_vec(const char *vec, size_t len) {
    return format_vec((const uint8_t *)vec, len);
}

std::string format_vec(const std::vector<uint8_t> &vec) {
    return format_vec(vec.data(), vec.size());
}

std::string format_vec(const uint16_t *vec, size_t len, const char *format) {
    std::string str{};
    char buf[16];

    if (!format)
        format = "%04hx";
    for (size_t n = 0; n < len; n++) {
        if (!str.empty())
            str.append(" ");
        std::snprintf(buf, sizeof(buf), format, vec[n]);
        str.append(buf);
    }
    return str;
}
