//
// Created by parfenkov on 21.07.18.
//

#include "argument_parser.h"


CArgumentParser::CArgumentParser() {

}

CArgumentParser::~CArgumentParser() {}

CDesktopResizer* CArgumentParser::Parse(char **argument) {
    scale = std::stoi(argument[1]);
    if (argument[2] == NULL) {
        mode.append("--dpi");
    } else {
        mode.append(argument[2]);
    }

    if (mode == "--dpi") {
        return new CDesktopResizerDpiMode();
    }

    throw std::runtime_error("Bad argument");
}


int& CArgumentParser::GetScale() {
    return scale;
}