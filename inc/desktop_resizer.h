//
// Created by parfenkov on 18.07.18.
//

#ifndef SCALINGUNIXSYSTEM_SCALING_SYSTEM_H
#define SCALINGUNIXSYSTEM_SCALING_SYSTEM_H


#include <X11/extensions/Xrandr.h>
#include <X11/Xlib.h>
#include <stdexcept>
#include <cstring>



class CDesktopResizer {
protected:
    Window window;
    Display *dpy;
    int screen;

    int pixelsToMillimeters(int pixels, double dpi);
    int millimetersToPixels(int mm, double dpi);


public:
    CDesktopResizer();
    ~CDesktopResizer();

    virtual void SetScale(int scale) {}

};


class CDesktopResizerDpiMode : public CDesktopResizer {
    void SetScale(int scale) override;
};


#endif //SCALINGUNIXSYSTEM_SCALING_SYSTEM_H
