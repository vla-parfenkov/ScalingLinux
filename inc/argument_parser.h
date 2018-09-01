//
// Created by parfenkov on 21.07.18.
//

#ifndef SCALINGUNIXSYSTEM_ARGUMENT_PARSER_H
#define SCALINGUNIXSYSTEM_ARGUMENT_PARSER_H


#include <iostream>
#include "desktop_resizer.h"
#include <map>

class CArgumentParser {
private:
    std::map<std::string, CDesktopResizer*> modes;
    std::string mode;
    uint32_t scale;
public:
    CArgumentParser();
    ~CArgumentParser();

    CDesktopResizer* Parse(char** argument);
    uint32_t& GetScale();
};


#endif //SCALINGUNIXSYSTEM_ARGUMENT_PARSER_H
