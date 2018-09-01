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
        scale = static_cast<uint32_t >(std::stoi(argument[1]));
    } catch (std::logic_error const &error) {
        throw std::invalid_argument("Bad argument");
    }

    if(scale < 50) {
        throw std::invalid_argument("Bad argument");
    }

    if (argument[2] == NULL) {
        mode.append("--scale");
    } else {
        mode.append(argument[2]);
    }

    try {
        return modes.at(mode);
    } catch (std::out_of_range const &error) {
            throw std::invalid_argument("Bad argument");
    }

}


uint32_t& CArgumentParser::GetScale() {
    return scale;
}