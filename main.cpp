#include <iostream>
#include <utility>
#include "main.h"
#include <fstream>

JSONParser::JSONParser(std::string f) : file_name(std::move(f)) {}

JSONParser::JSONParser() : JSONParser("input.json") {}

std::map<std::string, std::variant<std::string, bool, std::nullptr_t>> JSONParser::read() {
    std::ifstream file(file_name);

    if (!file) {
        throw std::runtime_error("failed to open file");
    }
    std::map<std::string, std::variant<std::string, bool, std::nullptr_t>> json;
    std::string current_key;
    std::string current_value;

    State state = State::Start;
    bool escape = false;
    int literal_index = 0;
    std::string true_literal = "true";
    std::string false_literal = "false";
    std::string null_literal = "null";
    std::string expected_literal;

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
                else if (chr == 't') {
                    state = State::InLiteral;
                    expected_literal = true_literal;
                    literal_index = 1;
                }
                else if (chr == 'f') {
                    state = State::InLiteral;
                    expected_literal = false_literal;
                    literal_index = 1;
                }
                else if (chr == 'n') {
                    state = State::InLiteral;
                    expected_literal = null_literal;
                    literal_index = 1;
                }
                else
                    state = State::Error;
                break;
            case State::InLiteral:
                if (literal_index < expected_literal.length() && expected_literal[literal_index] == chr) {
                    literal_index += 1;
                    if (literal_index == expected_literal.length()) {
                        if (expected_literal == "true")
                            json[current_key] = true;
                        else if (expected_literal == "false")
                            json[current_key] = false;
                        else if (expected_literal == "null") {
                            json[current_key] = nullptr;
                        }
                        current_key.clear();
                        state = State::ExpectCommaOrEnd;
                        literal_index = 0;
                    }
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
                if (chr == '"') {
                    json[current_key] = current_value;
                    current_key.clear();
                    current_value.clear();
                    state = State::ExpectCommaOrEnd;
                }
                else {
                    current_value.push_back(chr);
                }
                break;
            case State::ExpectCommaOrEnd:
                if (chr == ',')
                    state = State::ExpectKeyOrEnd;
                else if (chr == '}')
                    state = State::End;
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
    std::map<std::string, std::variant<std::string, bool, std::nullptr_t>> json = j.read();

    for (const auto& elem : json)
    {
        std::cout << elem.first << ": ";

        std::visit([](const auto& value)
        {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, std::string>)
            {
                std::cout << value;
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                std::cout << (value ? "true" : "false");
            }
            else if constexpr (std::is_same_v<T, std::nullptr_t>)
            {
                std::cout << "null";
            }
        }, elem.second);

        std::cout << "\n";
    }
    return 0;
}