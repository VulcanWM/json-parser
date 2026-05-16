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

#endif //JSON_PARSER_MAIN_H