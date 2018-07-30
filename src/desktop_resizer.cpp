//
// Created by parfenkov on 18.07.18.
//



#include "desktop_resizer.h"
#include <fstream>


CDesktopResizer::CDesktopResizer() {
    char *display_name = NULL;

    dpy = XOpenDisplay(display_name);

    if (dpy == NULL) {
        std::string error = "Can't open display ";
        error.append(XDisplayName(display_name));
        throw std::runtime_error(error);
    }

    screen  = DefaultScreen (dpy);
    if (screen >= ScreenCount(dpy)) {
        std::string error = "Invalid screen number ";
        error.append(std::to_string(screen));
        error.append("(display has ");
        error.append(std::to_string(ScreenCount(dpy)));
        throw std::runtime_error(error);
    }

    window = RootWindow(dpy, screen);
    XRRSelectInput (dpy, window, RRScreenChangeNotifyMask);
}

CDesktopResizer::~CDesktopResizer() {
    XCloseDisplay(dpy);
}


int CDesktopResizer::pixelsToMillimeters(int pixels, double dpi) {
    if(dpi <= 0) {
        std::string error = "Invalid dpi number ";
        throw std::runtime_error(error);
    }
    const double mm_per_inch = 25.4;
    return static_cast<int>((mm_per_inch * pixels)/dpi);
}


int CDesktopResizer::millimetersToPixels(int mm, double dpi) {
    const double mm_per_inch = 25.4;

    return static_cast<int>((dpi * mm) / mm_per_inch);
}

void CDesktopResizerDpiMode::SetScale(int scale) {
    double k_scaling = static_cast<double >(scale)/100.0;

    int display_height = DisplayHeight(dpy, screen);
    int display_width = DisplayWidth(dpy, screen);

    double dpi = (25.4 * DisplayHeight (dpy, screen)) / DisplayHeightMM(dpy, screen);

    dpi = dpi*k_scaling;
    int display_height_mm = pixelsToMillimeters(display_height, dpi);
    int display_width_mm = pixelsToMillimeters(display_width, dpi);

    XRRSetScreenSize(dpy, window, display_width, display_height, display_width_mm, display_height_mm);
    XSync(dpy, False);
}

