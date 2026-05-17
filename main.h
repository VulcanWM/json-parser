#ifndef JSON_PARSER_MAIN_H
#define JSON_PARSER_MAIN_H
#include <map>
#include <string>
#include <variant>

class JSONParser {
private:
    std::string file_name;
public:
    JSONParser(std::string f);
    JSONParser();

    std::map<std::string, std::variant<std::string, bool, std::nullptr_t, double>> read();
};

enum class State {
    Start,              // before {
    ExpectKeyOrEnd,     // after {
    InKey,              // inside "key"
    ExpectColon,        // after key
    ExpectValue,        // after :
    InLiteral,          // after : and started typing true or false
    InNumber,           // after : and started typing a number
    InValue,            // inside "value"
    ExpectCommaOrEnd,   // after value
    End,
    Error
};

#endif //JSON_PARSER_MAIN_H