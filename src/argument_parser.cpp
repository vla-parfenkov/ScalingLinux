//
// Created by parfenkov on 21.07.18.
//

#include "argument_parser.h"


CArgumentParser::CArgumentParser() {
    modes["--scale"] = new CDesktopResizerScaleMod();
    modes["--dpi"] = new CDesktopResizerDpiMode();
}

CArgumentParser::~CArgumentParser() {
    for(auto& item : modes)
    {
        delete item.second;
    }
}

CDesktopResizer* CArgumentParser::Parse(char **argument) {
    try {
        scale = std::stoi(argument[1]);
    } catch (std::logic_error) {
        throw std::invalid_argument("Bad argument");
    }

    if (argument[2] == NULL) {
        mode.append("--scale");
    } else {
        mode.append(argument[2]);
    }

    try {
        return modes.at(mode);
    } catch (std::out_of_range) {
            throw std::invalid_argument("Bad argument");
    }

}


int& CArgumentParser::GetScale() {
    return scale;
}