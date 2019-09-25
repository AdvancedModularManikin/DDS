#include "Utility.h"

namespace AMM {
    void Utility::add_once(std::vector<std::string> &vec, const std::string &element) {
        std::remove(vec.begin(), vec.end(), element);
        vec.push_back(element);
    }

    std::vector<std::string> Utility::explode(const std::string &delimiter,
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

    std::string Utility::decode64(const std::string &val) {
        using namespace boost::archive::iterators;
        using It =
        transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
        return boost::algorithm::trim_right_copy_if(
                std::string(It(std::begin(val)), It(std::end(val))),
                [](char c) { return c == '\0'; });
    }

    std::string Utility::encode64(const std::string &val) {
        using namespace boost::archive::iterators;
        using It =
        base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
        auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
        return tmp.append((3 - val.size() % 3) % 3, '=');
    }

    std::string Utility::getTimestampedFilename(const std::string &basePathname, const std::string &ext) {
        std::ostringstream filename;
        filename << basePathname << static_cast<unsigned long>(::time(0)) << ext;
        return filename.str();
    }

    std::map<std::string, std::vector<uint8_t>> Utility::parse_key_value(std::vector<uint8_t> kv) {
        std::map<std::string, std::vector<uint8_t>> m;
        bool keyfound = false;
        std::string key;
        std::vector<uint8_t> value;
        uint8_t prev = '\0';

        if (kv.size() == 0) {
            goto not_valid;
        }

        for (uint8_t u8 : kv) {
            if (keyfound) {
                if ((u8 == ';') && (prev != ';')) {
                    prev = u8;
                    continue;
                } else if ((u8 != ';') && (prev == ';')) {
                    if (value.size() == 0) {
                        goto not_valid;
                    }
                    m[key] = value;

                    key.clear();
                    value.clear();
                    keyfound = false;
                } else {
                    value.push_back(u8);
                }
            }
            if (!keyfound) {
                if (u8 == '=') {
                    if (key.size() == 0) {
                        goto not_valid;
                    }
                    keyfound = true;
                } else if (isalnum(u8)) {
                    key.push_back(u8);
                } else if ((u8 == '\0') && (key.size() == 0) && (m.size() > 0)) {
                    break; // accept trailing '\0' characters
                } else if ((prev != ';') || (key.size() > 0)) {
                    goto not_valid;
                }
            }
            prev = u8;
        }
        if (keyfound) {
            if (value.size() == 0) {
                goto not_valid;
            }
            m[key] = value;
        } else if (key.size() > 0) {
            goto not_valid;
        }
        return m;
        not_valid:
        // This is not a failure this is something that can happen because the
        // participant_qos userData
        // is used. Other participants in the system not created by rmw could use
        // userData for something
        // else.
        return std::map<std::string, std::vector<uint8_t>>();
    }
}