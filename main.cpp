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

    State state = State::Start;
    bool escape = false;

    char chr;

    while (file.get(chr) && state != State::End && state != State::Error) {
        if (std::isspace(static_cast<unsigned char>(chr)) && state != State::InKey && state != State::InValue)
            continue;

        switch (state) {
            case State::Start:
                if (chr == '{')
                    state = State::ExpectKeyOrEnd;
                else {
                    state = State::Error;
                }
                break;
            case State::ExpectKeyOrEnd:
                if (chr == '}')
                    state = State::End;
                else if (chr == '"') {
                    current_key.clear();
                    state = State::InKey;
                }
                else
                    state = State::Error;
                break;
            case State::InKey:
                if (escape) {
                    current_key.push_back(chr);
                    escape = false;
                    break;
                }
                if (chr == '\\') {
                    escape = true;
                    continue;
                }
                if (chr == '"')
                    state = State::ExpectColon;
                else {
                    current_key.push_back(chr);
                }
                break;
            case State::ExpectColon:
                if (chr == ':')
                    state = State::ExpectValue;
                else
                    state = State::Error;
                break;
            case State::ExpectValue:
                if (chr == '"') {
                    current_value.clear();
                    state = State::InValue;
                }
                else
                    state = State::Error;
                break;
            case State::InValue:
                if (escape) {
                    current_value.push_back(chr);
                    escape = false;
                    break;
                }
                if (chr == '\\') {
                    escape = true;
                    break;
                }
                if (chr == '"')
                    state = State::ExpectCommaOrEnd;
                else {
                    current_value.push_back(chr);
                }
                break;
            case State::ExpectCommaOrEnd:
                if (chr == ',') {
                    json[current_key] = current_value;
                    current_key.clear();
                    current_value.clear();
                    state = State::ExpectKeyOrEnd;
                }
                else if (chr == '}') {
                    json[current_key] = current_value;
                    current_key.clear();
                    current_value.clear();
                    state = State::End;
                }
                else
                    state = State::Error;
                break;
        }
    }

    if (state != State::End)
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