//
// Created by parfenkov on 03.08.18.
//

#ifndef SCALINGUNIXSYSTEM_SCREEN_RESOURCE_H
#define SCALINGUNIXSYSTEM_SCREEN_RESOURCE_H


#include <X11/extensions/Xrandr.h>

class CScreenResources {
private:

    XRRScreenResources* resources;
    RROutput rrOutput;
    Display* display;
    Window window;
    void release();

public:
    CScreenResources(Display* dpy, Window wind);
    ~CScreenResources();
    XRRScreenResources* Get();
    RRCrtc GetCrtc();
    RROutput GetOutput();
    RRMode GetIdForMode(const char* name);
    void Refresh();

};


#endif //SCALINGUNIXSYSTEM_SCREEN_RESOURCE_H
