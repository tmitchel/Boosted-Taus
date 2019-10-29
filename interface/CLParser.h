// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_CLPARSER_H_
#define INTERFACE_CLPARSER_H_

#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>

class CLParser {
 private:
    std::vector<std::string> tokens;
    std::vector<std::string> opts;

 public:
    CLParser(int&, char**);
    ~CLParser() {}
    bool Flag(const std::string&);
    std::string Option(const std::string&, const std::string);
    std::vector<std::string> MultiOption(const std::string&, int, int);
};

CLParser::CLParser(int &argc, char** argv) {
    // read tokens from argv
    for (int i = 0; i < argc; i++) {
        tokens.push_back(std::string(argv[i]));
    }
}

// parse flags (either present or not)
bool CLParser::Flag(const std::string &flag) {
    return std::find(tokens.begin(), tokens.end(), flag) != tokens.end();
}

// parse options (contain argument specifying details)
std::string CLParser::Option(const std::string &flag, const std::string default_value = "") {
    auto found = std::find(tokens.begin(), tokens.end(), flag);
    if (found != tokens.end() && ++found != tokens.end()) {
        return *found;
    }
    static const std::string empty(default_value);
    return empty;
}

// parse options (multiple parameters for this option)
std::vector<std::string> CLParser::MultiOption(const std::string &flag, int depth, int first = 1) {
    if (first == 1) {
        opts.clear();
    }
    if (depth == 0) {
        return opts;
    }
    auto found = std::find(tokens.begin(), tokens.end(), flag);
    if (found != tokens.end() && ++found != tokens.end()) {
        --depth;
        opts.push_back(*found);
        unsigned random;
        *found = *found + std::to_string(rand_r(&random));  // can't let it find this one again next time, so add a random ending
        MultiOption(*found, depth, 0);
    }
    return opts;
}

#endif  // INTERFACE_CLPARSER_H_
