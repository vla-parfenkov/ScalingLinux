//
// Created by parfenkov on 21.07.18.
//

#include "argument_parser.h"



CArgumentParser::CArgumentParser() {

    std::function<CDesktopResizer*()> scale_mode = []() { return CDesktopResizerScaleMod::Create(); };
    std::function<CDesktopResizer*()> dpi_mode = []() { return new CDesktopResizerXDpiMode(); };
    modes["--scale"] = scale_mode;
    modes["--dpi"] = dpi_mode;
}

CArgumentParser::~CArgumentParser() {
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

    if (argument[2] == nullptr) {
        mode.append("--scale");
    } else {
        mode.append(argument[2]);
    }

    try {
        return modes.at(mode)();
    } catch (std::out_of_range const &error) {
            throw std::invalid_argument("Bad argument");
    }




}


uint32_t& CArgumentParser::GetScale()  {
    return scale;
}

const std::string &CArgumentParser::GetMode() const {
    return mode;
}
