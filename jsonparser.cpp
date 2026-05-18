#include <utility>
#include "jsonparser.h"
#include <fstream>
#include <string>
#include <cctype>
#include <stdexcept>

JSONParser::JSONParser(std::string f) : file_name(std::move(f)) {}

JSONParser::JSONParser() : JSONParser("input.json") {}

std::map<std::string, json_value> JSONParser::read() {
    std::ifstream file(file_name);

    if (!file) throw std::runtime_error("failed to open file");
    std::map<std::string, json_value> json;
    std::string current_key;
    std::string current_value;
    std::string number_value;

    State state = State::Start;
    bool escape = false;
    bool has_dot = false;
    int literal_index = 0;
    std::string true_literal = "true";
    std::string false_literal = "false";
    std::string null_literal = "null";
    std::string expected_literal;
    std::vector<json_array*> array_stack;

    char chr;

    while (file.get(chr) && state != State::End && state != State::Error) {

        if (std::isspace(static_cast<unsigned char>(chr)) && state != State::InKey && state != State::InValue) continue;

        switch (state) {
            case State::Start:
                if (chr == '{') state = State::ExpectKeyOrEnd;
                else state = State::Error;
                break;
            case State::ExpectKeyOrEnd:
                if (chr == '}') state = State::End;
                else if (chr == '"') {
                    current_key.clear();
                    state = State::InKey;
                }
                else state = State::Error;
                break;
            case State::InKey:
                if (escape) {
                    current_key.push_back(chr);
                    break;
                }
                if (chr == '\\') {
                    escape = true;
                    continue;
                }
                if (chr == '"') state = State::ExpectColon;
                else current_key.push_back(chr);
                break;
            case State::ExpectColon:
                if (chr == ':') state = State::ExpectValue;
                else state = State::Error;
                break;
            case State::ExpectValue:
                if (chr == '"') {
                    current_value.clear();
                    escape = false;
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
                else if (chr == '-' || std::isdigit(chr)) {
                    state = State::InNumber;
                    has_dot = false;
                    number_value.clear();
                    number_value.push_back(chr);
                }
                else if (chr == '[') {
                    if (array_stack.empty()) {
                        json[current_key] = json_array{};
                        array_stack.push_back(&std::get<json_array>(json[current_key].value));
                        current_key.clear();
                    } else {
                        array_stack.back()->emplace_back(json_array{});
                        json_array& new_array =
                            std::get<json_array>(array_stack.back()->back().value);
                        array_stack.push_back(&new_array);
                    }

                    state = State::InArray;
                }
                else state = State::Error;
                break;
            case State::InArray:
                if (chr == '"') {
                    current_value.clear();
                    escape = false;
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
                else if (chr == '-' || std::isdigit(chr)) {
                    state = State::InNumber;
                    has_dot = false;
                    number_value.clear();
                    number_value.push_back(chr);
                }
                else if (chr == '[') {
                    array_stack.back()->emplace_back(json_array{});
                    json_array& new_array =
                        std::get<json_array>(array_stack.back()->back().value);
                    array_stack.push_back(&new_array);
                    state = State::InArray;
                }
                else if (chr == ']') {
                    array_stack.pop_back();
                    if (array_stack.empty())
                        state = State::ExpectCommaOrEnd;
                    else
                        state = State::ExpectCloseBracketOrComma;
                }
                else state = State::Error;
                break;
            case State::InLiteral:
                if (literal_index < expected_literal.length() && expected_literal[literal_index] == chr) {
                    literal_index += 1;
                    if (literal_index == expected_literal.length()) {
                        if (array_stack.empty()){
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
                        } else {
                            if (expected_literal == "true")
                                array_stack.back()->emplace_back(true);
                            else if (expected_literal == "false")
                                array_stack.back()->emplace_back(false);
                            else if (expected_literal == "null") {
                                array_stack.back()->emplace_back(nullptr);
                            }
                            state = State::ExpectCloseBracketOrComma;
                            literal_index = 0;
                        }
                    }
                }
                else state = State::Error;
                break;
            case State::InNumber:
                if (std::isdigit(chr) || chr == '.') {
                    if (chr == '.') {
                        if (has_dot)
                            state = State::Error;
                        has_dot = true;
                    }
                    number_value.push_back(chr);
                }
                else {
                    if (array_stack.empty()) {
                        json[current_key] = std::stod(number_value);
                        current_key.clear();
                        number_value.clear();

                        if (chr == ',') state = State::ExpectKeyOrEnd;
                        else if (chr == '}') state = State::End;
                        else state = State::Error;
                    } else {
                        array_stack.back()->emplace_back(std::stod(number_value));
                        number_value.clear();

                        if (chr == ',') {
                            state = State::InArray;
                        }
                        else if (chr == ']') {
                            array_stack.pop_back();

                            if (array_stack.empty())
                                state = State::ExpectCommaOrEnd;
                            else
                                state = State::ExpectCloseBracketOrComma;
                        }
                        else {
                            state = State::Error;
                        }
                    }
                }
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
                    if (array_stack.empty()){
                        json[current_key] = json_value{current_value};
                        current_key.clear();
                        current_value.clear();
                        state = State::ExpectCommaOrEnd;
                    } else {
                        array_stack.back()->emplace_back(current_value);
                        state = State::ExpectCloseBracketOrComma;
                    }
                }
                else current_value.push_back(chr);
                break;
            case State::ExpectCloseBracketOrComma:
                if (chr == ']') {
                    array_stack.pop_back();
                    if (array_stack.empty())
                        state = State::ExpectCommaOrEnd;
                    else
                        state = State::ExpectCloseBracketOrComma;
                } else if (chr == ',') state = State::InArray;
                else state = State::Error;
                break;
            case State::ExpectCommaOrEnd:
                if (chr == ',') state = State::ExpectKeyOrEnd;
                else if (chr == '}') state = State::End;
                else state = State::Error;
                break;
            case State::End:
            case State::Error:
                break;
        }
    }

    if (state != State::End) throw std::runtime_error("invalid json");

    return json;
}