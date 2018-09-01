//
// Created by parfenkov on 03.08.18.
//

#include <stdexcept>
#include <cstring>
#include "screen_resource.h"

CScreenResources::CScreenResources(Display* dpy, Window wind) : resources(nullptr), display(dpy), window(wind){

}

CScreenResources::~CScreenResources() {
  release();
}

XRRScreenResources *CScreenResources::Get() {
    return resources;
}

RRCrtc CScreenResources::GetCrtc() {
    return resources->crtcs[0];
}

RROutput CScreenResources::GetOutput() {
    return rrOutput;
}

RRMode CScreenResources::GetIdForMode(const char *name) {
    if(resources) {
        for (int i = 0; i < resources->nmode; ++i) {
            const XRRModeInfo &mode = resources->modes[i];
            if (strcmp(mode.name, name) == 0) {
                return mode.id;
            }
        }
    }
    return 0;
}

void CScreenResources::release() {
    if (resources) {
        XRRFreeScreenResources(resources);
        resources = nullptr;
    }
}

void CScreenResources::Refresh() {
    release();
    resources = XRRGetScreenResources(display, window);
    if(!resources) {
        std::string error = "Resource not found ";
        throw std::invalid_argument(error);
    }
    int xRandRMajor, xRandRMinor;
    XRRQueryVersion(display, &xRandRMajor, &xRandRMinor);

    if ((xRandRMajor == 1 && xRandRMinor >= 3) || xRandRMajor > 1)
    {
        rrOutput = XRRGetOutputPrimary(display, window);
    }
    else
    {
        rrOutput = resources->outputs[0];
    }
}



