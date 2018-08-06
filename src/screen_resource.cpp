//
// Created by parfenkov on 03.08.18.
//

#include <stdexcept>
#include <cstring>
#include "screen_resource.h"

CScreenResources::CScreenResources() : resources(NULL){

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
    return resources->outputs[0];
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
        resources = NULL;
    }
}

void CScreenResources::Refresh(Display *display, Window window) {
    release();
    resources = XRRGetScreenResources(display, window);
    if(!resources) {
        std::string error = "Resource not found ";
        throw std::invalid_argument(error);
    }
}

XRROutputInfo *CScreenResources::GetOutputInfo(Display *display, RROutput output_id) {
    return XRRGetOutputInfo(display, resources, output_id);
}
