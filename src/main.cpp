#include <iostream>
#include "desktop_resizer.h"
#include "argument_parser.h"


int main(int argc, char* argv[]) {
    CArgumentParser* argumentParser = nullptr;
    try{
        argumentParser = new CArgumentParser();
        auto desktopResizer = argumentParser->Parse(argv);
        desktopResizer->SetScale(argumentParser->GetScale());
    } catch (std::exception const &error) {
        std::cout << error.what() << std::endl;
    }

    if(argumentParser != nullptr) {
        delete argumentParser;
    }
    return 0;
}