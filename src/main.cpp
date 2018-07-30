#include <iostream>
#include "desktop_resizer.h"
#include "argument_parser.h"

int main(int argc, char* argv[]) {
    CArgumentParser* argumentParser = new CArgumentParser();

    CDesktopResizer* desktopResizer = argumentParser->Parse(argv);

    desktopResizer->SetScale(argumentParser->GetScale());

    delete desktopResizer;
    delete argumentParser;
    return 0;
}