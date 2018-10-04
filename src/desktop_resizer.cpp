//
// Created by parfenkov on 18.07.18.
//



#include "desktop_resizer.h"
#include <fstream>
#include <cmath>
#include <iostream>
#include <desktop_resizer.h>


CDesktopResizer::CDesktopResizer()  {
    char *display_name = nullptr;

    dpy = XOpenDisplay(display_name);

    if (dpy == nullptr) {
        std::string error = "Can't open display ";
        error.append(XDisplayName(display_name));
        throw std::logic_error(error);
    }

    screen  = DefaultScreen (dpy);
    if (screen >= ScreenCount(dpy)) {
        std::string error = "Invalid screen number ";
        error.append(std::to_string(screen));
        error.append("(display has ");
        error.append(std::to_string(ScreenCount(dpy)));
        throw std::logic_error(error);
    }

    window = RootWindow(dpy, screen);
    XRRSelectInput (dpy, window, RRScreenChangeNotifyMask);

    resources = new CScreenResources(dpy, window);

    try {
        resources->Refresh();
    } catch (std::invalid_argument const &error) {
        return;
    }

    XRRGetScreenSizeRange (dpy, window, &minWidth, &minHeight, &maxWidth, &maxHeight);

    //Set XErrorHandler
   // XSetErrorHandler(xErrorHandler);
}

CDesktopResizer::~CDesktopResizer() {
    XCloseDisplay(dpy);
    delete resources;
}


int CDesktopResizer::pixelsToMillimeters(int pixels, double dpi) {
    if(dpi <= 0) {
        throw std::invalid_argument("Invalid dpi number ");
    }
    return static_cast<int>((options::mm_per_inch * pixels)/dpi);
}


int CDesktopResizer::millimetersToPixels(int mm, double dpi) {
    return static_cast<int>((dpi * mm) / options::mm_per_inch);
}

/*This mod may not work in Ubuntu*/
void CDesktopResizerXDpiMode::SetScale(uint32_t scale) {
    double k_scaling = static_cast<double >(scale)/100.0;

    int display_height = DisplayHeight(dpy, screen);
    int display_width = DisplayWidth(dpy, screen);

    double dpi = tryToFindInitialDPI() * k_scaling;

    int display_height_mm = pixelsToMillimeters(display_height, dpi);
    int display_width_mm = pixelsToMillimeters(display_width, dpi);

    XRRSetScreenSize(dpy, window, display_width, display_height, display_width_mm, display_height_mm);
    XSync(dpy, False);
}


void CDesktopResizer::setScreenSize(const XTransform* transform) {
    int	    x, y, w, h;
    box	    bounds;

    point rect[4];
    rect[0].x = 0;
    rect[0].y = 0;
    rect[1].x = resources->Get()->modes->width;
    rect[1].y = 0;
    rect[2].x =resources->Get()->modes->width;
    rect[2].y = resources->Get()->modes->height;
    rect[3].x = 0;
    rect[3].y = resources->Get()->modes->height;
    int	    i;
    box   point;

    for (i = 0; i < 4; i++) {
        double	x, y;
        x = rect[i].x;
        y = rect[i].y;
        transformPoint(transform, &x, &y);
        point.x1 = static_cast<size_t >(std::floor (x));
        point.y1 = static_cast<size_t >(std::floor (y));
        point.x2 = static_cast<size_t >(std::ceil (x));
        point.y2 = static_cast<size_t >(std::ceil (y));
        if (i == 0)
            bounds = point;
        else {
            if (point.x1 < bounds.x1) bounds.x1 = point.x1;
            if (point.y1 < bounds.y1) bounds.y1 = point.y1;
            if (point.x2 > bounds.x2) bounds.x2 = point.x2;
            if (point.y2 > bounds.y2) bounds.y2 = point.y2;
        }
    }
    x = static_cast<int>(bounds.x1);
    y = static_cast<int>(bounds.y1);
    w = static_cast<int>(bounds.x2 - bounds.x1);
    h = static_cast<int>(bounds.y2 - bounds.y1);
    fb_width = x + w;
    fb_height = y + h;
    if (fb_width > maxWidth || fb_height > maxHeight) {
        throw std::invalid_argument("bad size");
    }
    if (fb_width < minWidth) {
        fb_width = minWidth;
    }
    if (fb_height < minHeight) {
        fb_height = minHeight;
    }

    int fb_width_mm;
    int fb_height_mm;

    if (fb_width != DisplayWidth (dpy, screen) || fb_height != DisplayHeight (dpy, screen) ) {
        double dpi = (options::mm_per_inch * DisplayHeight (dpy, screen)) / DisplayHeightMM(dpy, screen);
            fb_width_mm = static_cast<int>((options::mm_per_inch * fb_width) / dpi);
            fb_height_mm = static_cast<int>((options::mm_per_inch * fb_height) / dpi);
    } else {
            fb_width_mm = DisplayWidthMM (dpy, screen);
            fb_height_mm = DisplayHeightMM (dpy, screen);
    }


    XRRSetScreenSize (dpy, window, fb_width, fb_height, fb_width_mm, fb_height_mm);
}



void CDesktopResizer::transformPoint(const XTransform* transform, double* x, double* y) {
    double  vector[3];
    double  result[3];
    int	    i, j;
    double  v;

    vector[0] = *x;
    vector[1] = *y;
    vector[2] = 1;
    for (j = 0; j < 3; j++)
    {
        v = 0;
        for (i = 0; i < 3; i++)
            v += (XFixedToDouble (transform->matrix[j][i]) * vector[i]);
        result[j] = v;
    }
    if (!result[2])
        return;
    for (j = 0; j < 2; j++) {
        vector[j] = result[j] / result[2];
        if (vector[j] > INT16_MAX || vector[j] < INT16_MIN)
            return;
    }
    *x = vector[0];
    *y = vector[1];
}



void CDesktopResizer::setPanning(double scale) {
    if (scale < 1) {
        XRRPanning *pan = XRRGetPanning(dpy, resources->Get(), resources->GetCrtc());

        if (fb_width == 0 && fb_height == 0) {
            pan->width = static_cast<unsigned int>(resources->Get()->modes->width * scale);
            pan->height = static_cast<unsigned int>(resources->Get()->modes->height * scale);
        } else {
            pan->width = fb_width;
            pan->height = fb_height;
        }
        pan->top = pan->left = pan->track_left = pan->track_top = pan->track_width = 0;
        pan->track_height = pan->border_left = pan->border_top = pan->border_right = pan->border_bottom = 0;


        Status s = XRRSetPanning(dpy, resources->Get(), resources->GetCrtc(), pan);
        if (s != RRSetConfigSuccess) {
            XRRFreePanning(pan);
            throw std::logic_error("Set panning error");
        }
        XRRFreePanning(pan);
    }
}



double CDesktopResizerXDpiMode::tryToFindInitialDPI() {
    std::ifstream in(options::x_log_path);
    std::string data;
    double dpi = options::dpi_st;

    while (!in.eof()) {
        in >> data;
        if (data == "DPI") {
            in >> data;
            in >> data;
            if (data == "to") {
               in >> data;
                data.pop_back();
                data.erase(0, 1);
                data;
                try {
                    dpi = std::stoi(data);
                } catch (std::logic_error const &ex) {
                    dpi = options::dpi_st;
                }
                break;
            }
        }
    }
    in.close();
    return dpi;
}

CDesktopResizerXDpiMode::CDesktopResizerXDpiMode() : CDesktopResizer() {
    if(std::getenv(options::wayland_display_env.c_str()) != nullptr) {
        throw std::logic_error("DPI mode not supported in Wayland");
    }
}

void CDesktopResizerXScaleMode::SetScale(uint32_t scale) {
    int major, minor;
    XRRQueryVersion(dpy, &major, &minor);

    if (major < 1 || (major == 1 && minor < 3)) {
        throw std::logic_error("RandR version < 1.3 don't supported scaling");
    }
    XGrabServer(dpy);
    XSynchronize(dpy, true);
    XTransform transform;
    std::string filter;
    double k_scaling = static_cast<double >(scale)/100.0;
    k_scaling = 1.0/k_scaling;

    std::memset (&transform, '\0', sizeof (transform));
    transform.matrix[0][0] = XDoubleToFixed (k_scaling);
    transform.matrix[1][1] = XDoubleToFixed (k_scaling);
    transform.matrix[2][2] = XDoubleToFixed (1.0);

    if (k_scaling != 1) {
        filter.append("bilinear");
    } else {
        filter.append("nearest");
    }

    RROutput output = resources->GetOutput();
    XRRSetCrtcConfig (dpy, resources->Get(), resources->GetCrtc(), CurrentTime, 0, 0, None, RR_Rotate_0, NULL, 0);
    try {
        setScreenSize(&transform);
        XRRSetCrtcTransform(dpy, resources->GetCrtc(), &transform, filter.c_str(), NULL, 0);
        Status s = XRRSetCrtcConfig(dpy, resources->Get(), resources->GetCrtc(),
                CurrentTime, 0, 0, resources->Get()->modes[0].id, RR_Rotate_0,
                &output, 1);
        if (s != RRSetConfigSuccess) {
            rollBack();
        } else {
            setPanning(static_cast<double >(scale) / 100.0);
        }
    } catch (std::exception const &error) {
        std::cerr << error.what() << std::endl;
        rollBack();
    }
    XUngrabServer(dpy);
    XSynchronize(dpy, false);
    XSync (dpy, False);
}

void CDesktopResizer::rollBack() {


    for (uint32_t c = 0; c < resources->Get()->ncrtc; c++) {
        RRCrtc crtc = resources->Get()->crtcs[c];
        XRRSetCrtcConfig(dpy, resources->Get(), crtc, CurrentTime,
                0, 0, None, RR_Rotate_0, NULL, 0);
    }

    XRRSetScreenSize (dpy, window,
                      DisplayWidth (dpy, screen),
                      DisplayHeight (dpy, screen),
                      DisplayWidthMM (dpy, screen),
                      DisplayHeightMM (dpy, screen));


    for (uint32_t c = 0; c < resources->Get()->ncrtc; c++) {
        XRRCrtcTransformAttributes *attributes = nullptr;
        RRCrtc crtc = resources->Get()->crtcs[c];
        XRRCrtcInfo* crtcInfo = XRRGetCrtcInfo(dpy, resources->Get(), crtc);
        XRRGetCrtcTransform(dpy, crtc, &attributes);
        XRRSetCrtcTransform(dpy, crtc, &attributes->currentTransform, const_cast<const char*>(attributes->currentFilter),
                attributes->currentParams, attributes->currentNparams);
        XRRSetCrtcConfig (dpy, resources->Get(), crtc, CurrentTime,
                                 crtcInfo->x, crtcInfo->y,
                                 crtcInfo->mode, crtcInfo->rotation,
                                 crtcInfo->outputs, crtcInfo->noutput);
        XRRFreeCrtcInfo(crtcInfo);
        XFree(attributes);
    }


}

/*This XErrorHandler can be use for logging X error*/
/*int CDesktopResizer::xErrorHandler(Display *dpy, XErrorEvent *error) {
    std::cerr << "Error code: " << error->error_code << std::endl;
    std::cerr << "Error text: " << XGetErrorText(dpy, error->error_code, buffer, bufferSize) << std::endl;
    return 0;
}*/


CDesktopResizerScaleMod *CDesktopResizerScaleMod::Create() {
    if((std::getenv(options::wayland_display_env.c_str()) == nullptr)
        && (std::getenv(options::display_env.c_str()) != nullptr)) {
        return new CDesktopResizerXScaleMode();
    }

    if (CDBusConnection::IsInterface(mutter::interface)) {
        return new CDesktopResizerMutterScaleMode();
    }
    throw std::logic_error ("Display type not found or unknown display type");

}

void CDesktopResizerMutterScaleMode::SetScale(uint32_t scale) {
    GVariant* variant = nullptr;

    setScaleMonitorFramebufferMode();

    variant = dBusConnection->CallMethod(mutter::interface, mutter::object, mutter::interface,
            mutter::methodGetState, nullptr);

    auto mutterConfigDisplay = new CMutterConfigDisplay(variant);

    double k_scaling = static_cast<double >(scale)/100.0;

    mutterConfigDisplay->SetScale(k_scaling);

    dBusConnection->CallMethod(mutter::interface,mutter:: object, mutter::interface, mutter::methodApply, mutterConfigDisplay->CreateApplyArgument());

    g_variant_unref (variant);
    delete mutterConfigDisplay;
}

CDesktopResizerMutterScaleMode::CDesktopResizerMutterScaleMode() {
    dBusConnection = new CDBusConnection();
}

CDesktopResizerMutterScaleMode::~CDesktopResizerMutterScaleMode() {
    delete  dBusConnection;
}

void CDesktopResizerMutterScaleMode::setScaleMonitorFramebufferMode() {
    GSettings* settings = g_settings_new(mutter::gsetting_shema.c_str());
    char** old =  g_settings_get_strv(settings, mutter::experimental_features.c_str());
    std::string scaleMonitorBuffer = "scale-monitor-framebuffer";
    char** array;
    uint32_t size = 0;

    for (size = 0; old[size] != NULL; size++) {
        if(strcmp((scaleMonitorBuffer + "\n").c_str(), *old)) {
            g_strfreev(old);
            return;
        }
    }

    size += 1; 
    size += 1; 

    array = (char**)g_realloc_n (old, size, sizeof (char *));

    array[size - 2] = g_strdup (scaleMonitorBuffer.c_str());
    array[size - 1] = NULL;


    g_settings_set_strv(settings, mutter::experimental_features.c_str(), array);
    g_settings_apply(settings);
    g_settings_sync();
    std::cout << "The mutter experimental-features mode is on" << std::endl;
    g_strfreev(array);
}
