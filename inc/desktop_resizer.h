//
// Created by parfenkov on 18.07.18.
//

#ifndef SCALINGUNIXSYSTEM_SCALING_SYSTEM_H
#define SCALINGUNIXSYSTEM_SCALING_SYSTEM_H


#include <X11/extensions/Xrandr.h>
#include <X11/Xlib.h>
#include <stdexcept>
#include <cstring>
#include "screen_resource.h"



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

class CDesktopResizerScaleMod : public CDesktopResizer {
    void SetScale(int scale) override;

private:

    void createMode(const char* name, int width, int height, CScreenResources* resources);
    void deleteMode(const char* name, CScreenResources* resources);
};


#endif //SCALINGUNIXSYSTEM_SCALING_SYSTEM_H
