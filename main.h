#ifndef JSON_PARSER_MAIN_H
#define JSON_PARSER_MAIN_H
#include <map>
#include <string>

class JSONParser {
private:
    std::string file_name;
public:
    JSONParser(std::string f);
    JSONParser();

    std::map<std::string, std::string> read();
};

enum class State {
    Start,              // before {
    ExpectKeyOrEnd,     // after {
    InKey,              // inside "key"
    ExpectColon,        // after key
    ExpectValue,        // after :
    InValue,            // inside "value"
    ExpectCommaOrEnd,   // after value
    End,
    Error
};

#endif //JSON_PARSER_MAIN_H