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


namespace options {
    const double mm_per_inch = 25.4;
    const double dpi_st = 96.0;
    const std::string log_path = "/var/log/Xorg.0.log";
}

typedef struct {
    size_t	    x1, y1, x2, y2;
} box;

typedef struct {
    size_t	    x, y;
} point;


class CDesktopResizer {
protected:
    bool grab;
    Window window;
    Display *dpy;
    int screen;
    CScreenResources* resources;
    int maxWidth;
    int maxHeight;
    int minWidth;
    int minHeight;
    int fb_width, fb_height;


    int pixelsToMillimeters(int pixels, double dpi);
    int millimetersToPixels(int mm, double dpi);
    void  setScreenSize(const XTransform* transform);
    void transformPoint(const XTransform* transform, double* x, double* y);
    void setPanning(double scale);


public:
    CDesktopResizer();
    ~CDesktopResizer();

    virtual void SetScale(uint32_t scale) {}

};


class CDesktopResizerDpiMode : public CDesktopResizer {
public:
    void SetScale(uint32_t scale) override;

private:
    double tryToFindInitialDPI();
};

class CDesktopResizerScaleMod : public CDesktopResizer {
public:
    void SetScale(uint32_t scale) override;
};


#endif //SCALINGUNIXSYSTEM_SCALING_SYSTEM_H
