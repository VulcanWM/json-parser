#include <iostream>
#include <utility>
#include "jsonparser.h"
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "usage: ./program <filename>\n";
        return 1;
    }

    JSONParser j(argv[1]);
    std::map<std::string, json_value> json = j.read();

    for (const auto& [key, val] : json) {
        std::cout << key << ": ";

        std::visit([&](const auto& value) {
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
        }, val.value);

        std::cout << "\n";
    }
    return 0;
}