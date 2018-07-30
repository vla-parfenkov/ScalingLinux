//
// Created by parfenkov on 21.07.18.
//

#ifndef SCALINGUNIXSYSTEM_ARGUMENT_PARSER_H
#define SCALINGUNIXSYSTEM_ARGUMENT_PARSER_H


#include <iostream>
#include "desktop_resizer.h"

class CArgumentParser {
private:
    std::string mode;
    int scale;
public:
    CArgumentParser();
    ~CArgumentParser();

    CDesktopResizer* Parse(char** argument);
    int& GetScale();
};


#endif //SCALINGUNIXSYSTEM_ARGUMENT_PARSER_H
