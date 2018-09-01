#include <iostream>
#include "desktop_resizer.h"
#include "argument_parser.h"

int main(int argc, char* argv[]) {
    auto argumentParser = new CArgumentParser();

    try {
        auto desktopResizer = argumentParser->Parse(argv);
        desktopResizer->SetScale(argumentParser->GetScale());
    } catch (std::logic_error const &error) {
        std::cout << error.what() << std::endl;
    }

    delete argumentParser;
    return 0;
}