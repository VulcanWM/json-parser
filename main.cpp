#include <iostream>
#include <utility>
#include "main.h"
#include <fstream>

JSONParser::JSONParser(std::string f) : file_name(std::move(f)) {}

JSONParser::JSONParser() : JSONParser("input.json") {}

std::map<std::string, std::string> JSONParser::read() {
    std::ifstream file(file_name);

    if (!file) {
        throw std::runtime_error("failed to open file");
    }
    std::map<std::string, std::string> json;
    std::string current_key;
    std::string current_value;
    bool parsing_error = false;
    bool parsing_finished = false;
    bool has_been_written = true;

    bool in_json = false;
    bool in_key = false;
    bool in_value = false;
    bool colon_seen = false;
    bool escape = false;

    char chr;

    while (file.get(chr) && parsing_error == false && parsing_finished == false) {
        if (std::isspace(chr) && !in_key && !in_value)
            continue;

        if (in_key && chr == '\\' && !escape){
            escape = true;
            continue;
        }

        if (in_value && chr == '\\' && !escape) {
            escape = true;
            continue;
        }

        if (escape) {
            if (in_key)
                current_key.push_back(chr);
            else if (in_value)
                current_value.push_back(chr);
            escape = false;
            continue;
        }

        if (in_key && chr != '"')
            current_key.push_back(chr);

        else if (in_value && chr != '"')
            current_value.push_back(chr);

        if (chr == '{' && !in_json)
            in_json = true;

        else if (chr == '{' && in_json)
            parsing_error = true;

        else if (chr == '"' && !in_key && !colon_seen && !in_value) {
            in_key = true;
            has_been_written = false;
        }

        else if (chr == '"' && in_key && !colon_seen && !in_value)
            in_key = false;

        else if (chr == ':' && !in_key && !colon_seen && !in_value)
            colon_seen = true;

        else if (chr == ':' && colon_seen)
            parsing_error = true;

        else if (chr == '"' && colon_seen && !in_key && !in_value)
            in_value = true;

        else if (chr == '"' && colon_seen && !in_key && in_value)
            in_value = false;

        else if (chr == ',' && colon_seen && !in_key && !in_value) {
            colon_seen = false;
            json[current_key] = current_value;
            current_key.clear();
            current_value.clear();
            has_been_written = true;
        }
        else if (in_json && chr == '}') {
            parsing_finished = true;
            if (!has_been_written) {
                json[current_key] = current_value;
            }
        }
    }

    if (!parsing_finished || parsing_error)
        throw std::runtime_error("invalid json");

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
        std::cout << elem.first << ": " << elem.second << "\n";
    }
    return 0;
}