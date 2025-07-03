#include "../include/string_utils.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <regex>

std::string StringUtils::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtils::toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string StringUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

std::string StringUtils::reverse(const std::string& str) {
    std::string result = str;
    std::reverse(result.begin(), result.end());
    return result;
}

bool StringUtils::isPalindrome(const std::string& str) {
    std::string cleaned = toLowerCase(trim(str));
    cleaned.erase(std::remove_if(cleaned.begin(), cleaned.end(), 
                 [](char c) { return !std::isalnum(c); }), cleaned.end());
    
    return cleaned == reverse(cleaned);
}

int StringUtils::countWords(const std::string& str) {
    if (trim(str).empty()) return 0;
    
    std::istringstream iss(str);
    std::string word;
    int count = 0;
    while (iss >> word) {
        count++;
    }
    return count;
}

int StringUtils::countVowels(const std::string& str) {
    int count = 0;
    for (char c : str) {
        char lower = std::tolower(c);
        if (lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u') {
            count++;
        }
    }
    return count;
}

bool StringUtils::isValidEmail(const std::string& email) {
    const std::regex pattern(R"((\w+)(\.?\w+)*@(\w+)(\.?\w+)+)");
    return std::regex_match(email, pattern);
}

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string StringUtils::join(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) return "";
    
    std::string result = strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        result += delimiter + strings[i];
    }
    return result;
}

std::string StringUtils::replace(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

bool StringUtils::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool StringUtils::isAlphabetic(const std::string& str) {
    if (str.empty()) return false;
    return std::all_of(str.begin(), str.end(), ::isalpha);
}

bool StringUtils::isAlphanumeric(const std::string& str) {
    if (str.empty()) return false;
    return std::all_of(str.begin(), str.end(), ::isalnum);
} 