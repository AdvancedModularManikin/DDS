#pragma once

#ifndef AMM_UTILITY_H
#define AMM_UTILITY_H

#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

std::string get_random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] = "0123456789"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

void add_once(std::vector<std::string> &vec, const std::string &element) {
    std::remove(vec.begin(), vec.end(), element);
    vec.push_back(element);
}

std::vector<std::string> explode(const std::string &delimiter,
                                 const std::string &str) {
    std::vector<std::string> arr;

    int strleng = str.length();
    int delleng = delimiter.length();
    if (delleng == 0)
        return arr; // no change

    int i = 0;
    int k = 0;
    while (i < strleng) {
        int j = 0;
        while (i + j < strleng && j < delleng && str[i + j] == delimiter[j])
            j++;
        if (j == delleng) // found delimiter
        {
            arr.push_back(str.substr(k, i - k));
            i += delleng;
            k = i;
        } else {
            i++;
        }
    }
    arr.push_back(str.substr(k, i - k));
    return arr;
};

std::string decode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It =
    transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
    return boost::algorithm::trim_right_copy_if(
            std::string(It(std::begin(val)), It(std::end(val))),
            [](char c) { return c == '\0'; });
}

std::string encode64(const std::string &val) {
    using namespace boost::archive::iterators;
    using It =
    base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
    return tmp.append((3 - val.size() % 3) % 3, '=');
}

std::string getTimestampedFilename(const std::string &basePathname, const std::string &ext) {
    std::ostringstream filename;
    filename << basePathname << static_cast<unsigned long>(::time(0)) << ext;
    return filename.str();
}

#endif
