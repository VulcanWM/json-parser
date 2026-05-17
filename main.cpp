#include <iostream>
#include <utility>
#include "jsonparser.cpp"
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "usage: ./program <filename>\n";
        return 1;
    }

    JSONParser j(argv[1]);
    std::map<std::string, std::variant<std::string, bool, std::nullptr_t, double>> json = j.read();

    for (const auto& elem : json)
    {
        std::cout << elem.first << ": ";

        std::visit([](const auto& value)
        {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, std::string>) std::cout << value;
            else if constexpr (std::is_same_v<T, bool>) std::cout << (value ? "true" : "false");
            else if constexpr (std::is_same_v<T, std::nullptr_t>) std::cout << "null";
            else if constexpr (std::is_same_v<T, double>) std::cout << std::to_string(value);
        }, elem.second);

        std::cout << "\n";
    }
    return 0;
}