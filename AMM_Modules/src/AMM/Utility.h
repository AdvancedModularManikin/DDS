#pragma once

#include "stdafx.h"
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

namespace AMM {
    class Utility {
    public:
        static void add_once(std::vector<std::string> &vec, const std::string &element);

        static std::vector<std::string> explode(const std::string &delimiter,
                                         const std::string &str);

        static std::string decode64(const std::string &val);

        static std::string encode64(const std::string &val);

        static std::string getTimestampedFilename(const std::string &basePathname, const std::string &ext);

        static std::map<std::string, std::vector<uint8_t>> parse_key_value(std::vector<uint8_t> kv);
    };
}