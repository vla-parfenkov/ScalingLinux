//
// Created by parfenkov on 18.07.18.
//



#include "desktop_resizer.h"
#include <fstream>
#include <cmath>
#include <iostream>


CDesktopResizer::CDesktopResizer() : grab(false) {
    char *display_name = nullptr;

    dpy = XOpenDisplay(display_name);

    if (dpy == nullptr) {
        std::string error = "Can't open display ";
        error.append(XDisplayName(display_name));
        throw std::runtime_error(error);
    }

    screen  = DefaultScreen (dpy);
    if (screen >= ScreenCount(dpy)) {
        std::string error = "Invalid screen number ";
        error.append(std::to_string(screen));
        error.append("(display has ");
        error.append(std::to_string(ScreenCount(dpy)));
        throw std::runtime_error(error);
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
}

CDesktopResizer::~CDesktopResizer() {
    XCloseDisplay(dpy);
    delete resources;
}


int CDesktopResizer::pixelsToMillimeters(int pixels, double dpi) {
    if(dpi <= 0) {
        std::string error = "Invalid dpi number ";
        throw std::runtime_error(error);
    }
    return static_cast<int>((options::mm_per_inch * pixels)/dpi);
}


int CDesktopResizer::millimetersToPixels(int mm, double dpi) {
    return static_cast<int>((dpi * mm) / options::mm_per_inch);
}

void CDesktopResizerDpiMode::SetScale(uint32_t scale) {
    double k_scaling = static_cast<double >(scale)/100.0;

    int display_height = DisplayHeight(dpy, screen);
    int display_width = DisplayWidth(dpy, screen);

    double dpi = tryToFindInitialDPI() * k_scaling;

    int display_height_mm = pixelsToMillimeters(display_height, dpi);
    int display_width_mm = pixelsToMillimeters(display_width, dpi);

    XRRSetScreenSize(dpy, window, display_width, display_height, display_width_mm, display_height_mm);
    XSync(dpy, False);
}


void CDesktopResizerScaleMod::SetScale(uint32_t scale) {
    XGrabServer(dpy);
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
        if (s == RRSetConfigSuccess) {
            setPanning(static_cast<double >(scale) / 100.0);
        }
    } catch (std::runtime_error const &error) {
        XRRSetCrtcConfig(dpy, resources->Get(), resources->GetCrtc(),
                         CurrentTime, 0, 0, resources->Get()->modes[0].id, RR_Rotate_0,
                         &output, 1);
    }
    XUngrabServer(dpy);
    XSync (dpy, False);
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

        int major, minor;

        XRRQueryVersion(dpy, &major, &minor);

        if(major > 1 || (major == 1 && minor >= 3)) {
            Status s = XRRSetPanning(dpy, resources->Get(), resources->GetCrtc(), pan);
            if (s != RRSetConfigSuccess) {
                throw std::logic_error("Set panning error");
            }
        }
        XFree(pan);
    }
}


double CDesktopResizerDpiMode::tryToFindInitialDPI() {
    std::ifstream in(options::log_path);
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