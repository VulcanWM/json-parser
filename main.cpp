#include <iostream>
#include <utility>
#include "main.h"
#include <fstream>

JSONParser::JSONParser(std::string f) : file_name(std::move(f)) {}

JSONParser::JSONParser() : JSONParser("input.json") {}

std::map<std::string, std::string> JSONParser::read() {
    std::ifstream file(file_name);
    std::map<std::string, std::string> map;
    std::string current_key;
    std::string current_value;
    bool in_json = false;
    bool in_key = false;
    bool in_value = false;
    bool colon_seen = false;

    char ch;

    std::cout << "Full file content:\n";
    while (file.get(ch)) {
        std::cout << ch;
    }

    return map;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "usage: ./program <filename>\n";
        return 1;
    }

    JSONParser j(argv[1]);
    std::map<std::string, std::string> p = j.read();

    return 0;
}