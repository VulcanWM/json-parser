#include <iostream>
#include <utility>
#include "main.h"
#include <fstream>

JSONParser::JSONParser(std::string f) : file_name(std::move(f)) {}

JSONParser::JSONParser() : JSONParser("input.json") {}

std::map<std::string, std::string> JSONParser::read() {
    std::ifstream file(file_name);
    std::map<std::string, std::string> json;
    std::string current_key;
    std::string current_value;
    bool parsing_error = false;
    bool parsing_finished = false;

    bool in_json = false;
    bool in_key = false;
    bool in_value = false;
    bool colon_seen = false;

    char chr;

    while (file.get(chr) && parsing_error == false && parsing_finished == false) {

        if (in_key == true) {
            current_key.push_back(chr);
        }
        else if (in_value == true) {
            current_value.push_back(chr);
        }

        if (chr == '{' and in_json == false) {
            in_json = true;
        }
        else if (chr == '{' && in_json == true) {
            parsing_error = true;
        }
        else if (chr == '"' && in_key == false && colon_seen == false && in_value == false) {
            in_key = true;
        }
        else if (chr == '"' && in_key == true && colon_seen == false && in_value == false) {
            current_key.pop_back();
            in_key = false;
        }
        else if (chr == ':' && in_key == false && colon_seen == false && in_value == false) {
            colon_seen = true;
        }
        else if (chr == ':' && colon_seen == true) {
            parsing_error = true;
        }
        else if (chr == '"' && colon_seen == true && in_key == false && in_value == false) {
            in_value = true;
        }
        else if (chr == '"' && colon_seen == true && in_key == false && in_value == true) {
            current_value.pop_back();
            in_value = false;
        }
        else if (chr == ',' && colon_seen == true && in_key == false && in_value == false) {
            colon_seen = false;
            json.insert({current_key, current_value});
            current_key = "";
            current_value = "";
        }
        else if (in_json == true && chr == '}') {
            parsing_finished = true;
            json.insert({current_key, current_value});
        }
    }

    return json;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "usage: ./program <filename>\n";
        return 1;
    }

    JSONParser j(argv[1]);
    std::map<std::string, std::string> json = j.read();

    for(const auto& elem : json)
    {
        std::cout << elem.first << " " << elem.second << "\n";
    }
    return 0;
}