#include <iostream>
#include <utility>
#include "jsonparser.h"
#include <string>

void print_json_value(const json_value& val);

void print_json_value(const json_value& val) {
    std::visit([](const auto& value) {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, std::string>) {
            std::cout << value;
        }
        else if constexpr (std::is_same_v<T, bool>) {
            std::cout << (value ? "true" : "false");
        }
        else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            std::cout << "null";
        }
        else if constexpr (std::is_same_v<T, double>) {
            std::cout << value;
        }
        else if constexpr (std::is_same_v<T, json_array>) {
            std::cout << "[";
            for (size_t i = 0; i < value.size(); ++i) {
                print_json_value(value[i]);   // recursive call
                if (i + 1 < value.size())
                    std::cout << ", ";
            }
            std::cout << "]";
        }

    }, val.value);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "usage: ./program <filename>\n";
        return 1;
    }

    JSONParser j(argv[1]);
    std::map<std::string, json_value> json = j.read();

    for (const auto& [key, val] : json) {
        std::cout << key << ": ";
        print_json_value(val);
        std::cout << "\n";
    }

    return 0;
}