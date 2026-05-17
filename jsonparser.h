#ifndef JSON_PARSER_JSONPARSER_H
#define JSON_PARSER_JSONPARSER_H
#include <map>
#include <string>
#include <variant>

struct json_value;

using json_array = std::vector<std::shared_ptr<json_value>>;

struct json_value {
    using value_t = std::variant<
        std::string,
        bool,
        std::nullptr_t,
        double,
        json_array
    >;

    value_t value;

    json_value() = default;

    json_value(const std::string& v) : value(v) {}
    json_value(bool v) : value(v) {}
    json_value(std::nullptr_t v) : value(v) {}
    json_value(double v) : value(v) {}
    json_value(const json_array& v) : value(v) {}
};

class JSONParser {
private:
    std::string file_name;
public:
    JSONParser(std::string f);
    JSONParser();

    std::map<std::string, json_value> read();
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

#endif //JSON_PARSER_JSONPARSER_H