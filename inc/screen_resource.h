//
// Created by parfenkov on 03.08.18.
//

#ifndef SCALINGUNIXSYSTEM_SCREEN_RESOURCE_H
#define SCALINGUNIXSYSTEM_SCREEN_RESOURCE_H


#include <X11/extensions/Xrandr.h>

class CScreenResources {
private:
    XRRScreenResources* resources;
    void release();

public:
    CScreenResources();
    ~CScreenResources();
    XRRScreenResources* Get();
    RRCrtc GetCrtc();
    RROutput GetOutput();
    RRMode GetIdForMode(const char* name);
    void Refresh(Display* display, Window window);
    XRROutputInfo* GetOutputInfo(Display* display, RROutput output_id);

};


#endif //SCALINGUNIXSYSTEM_SCREEN_RESOURCE_H
