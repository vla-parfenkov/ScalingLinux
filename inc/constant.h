//
// Created by vladislav on 04.09.18.
//

#ifndef LSCALING_CONSTANT_H
#define LSCALING_CONSTANT_H

#include <iostream>

namespace format_mutter_interface {
    const std::string mode = "(siiddada{sv})";
    const std::string modes = "a" + mode;
    const std::string monitor_spec = "(ssss)";
    const std::string monitors_spec = "a" + monitor_spec;
    const std::string monitor = "(" + monitor_spec + modes + "a{sv})";
    const std::string monitors = "a" + monitor;
    const std::string logical_monitor = "(iiduba" + monitor_spec +  "a{sv})";
    const std::string logical_monitors = "a" + logical_monitor;
    const std::string current_state = "(u" + monitors + logical_monitors +  "a{sv})";
    const uint8_t dpy_layout_mode_logical = 1;
    const uint8_t dpy_layout_mode_physical = 2;
}

namespace options {
    const double mm_per_inch = 25.4;
    const double dpi_st = 96.0;
    const std::string x_log_path = "/var/log/Xorg.0.log";
    const std::string wayland_display_env = "WAYLAND_DISPLAY";
    const std::string display_env = "DISPLAY";
}

namespace mutter {
    const std::string interface = "org.gnome.Mutter.DisplayConfig";
    const std::string object =  "/org/gnome/Mutter/DisplayConfig";
    const std::string methodGetState =  "GetCurrentState";
    const std::string methodApply = "ApplyMonitorsConfig";
}


#endif //LSCALING_CONSTANT_H
