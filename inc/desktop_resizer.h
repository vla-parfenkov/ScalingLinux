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
#include "constant.h"
#include "dbus_connection.h"
#include "mutter_config_display.h"



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


    virtual int pixelsToMillimeters(int pixels, double dpi);
    int millimetersToPixels(int mm, double dpi);
    void  setScreenSize(const XTransform* transform);
    void transformPoint(const XTransform* transform, double* x, double* y);
    void setPanning(double scale);


public:
    CDesktopResizer();

    ~CDesktopResizer();

    virtual void SetScale(uint32_t scale) {}

};


class CDesktopResizerXDpiMode : public CDesktopResizer {
public:
    void SetScale(uint32_t scale) override;

    CDesktopResizerXDpiMode();

private:
    double tryToFindInitialDPI();
};


class CDesktopResizerScaleMod : public CDesktopResizer {
public:
    static CDesktopResizerScaleMod* Create();
};


class CDesktopResizerXScaleMode : public CDesktopResizerScaleMod {
public:
    void SetScale(uint32_t scale) override;
};


class CDesktopResizerMutterScaleMode : public CDesktopResizerScaleMod {
private:
    CDBusConnection* dBusConnection;
public:
    CDesktopResizerMutterScaleMode();

    ~CDesktopResizerMutterScaleMode();
    void SetScale(uint32_t scale) override;
};

#endif //SCALINGUNIXSYSTEM_SCALING_SYSTEM_H
