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
}

typedef struct {
    size_t	    x1, y1, x2, y2;
} box;

typedef struct {
    size_t	    x, y;
} point;


class CDesktopResizer {
protected:
    Window window;
    Display *dpy;
    int screen;
    CScreenResources* resources;
    int maxWidth;
    int maxHeight;
    int minWidth;
    int minHeight;


    int pixelsToMillimeters(int pixels, double dpi);
    int millimetersToPixels(int mm, double dpi);
    void  setScreenSize(const XTransform* transform);
    void transformPoint(const XTransform* transform, double* x, double* y);
    void setPanning(double scale);


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

    void createMode(const char* name, int width, int height);
    void deleteMode(const char* name);

};


#endif //SCALINGUNIXSYSTEM_SCALING_SYSTEM_H
