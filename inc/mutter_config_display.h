//
// Created by vladislav on 02.09.18.
//

#ifndef LSCALING_WAYLAND_MUTTER_CONFIG_DISPLAY_H
#define LSCALING_WAYLAND_MUTTER_CONFIG_DISPLAY_H


#include <stdint-gcc.h>
#include <list>
#include <iostream>
#include <gio/gio.h>
#include <map>
#include "constant.h"

class CMutterMode {
private:
    std::string id;
    uint32_t width;
    uint32_t height;
    double refreshRate;
    double preferredScale;
    std::list<double > supportedScales;
    bool isCurrent;

    void parseProp(GVariantIter* prop);
public:
    CMutterMode();
    ~CMutterMode();

    std::string getId();

    bool isIsCurrent() const;

    explicit CMutterMode(GVariant* variant);


    uint32_t getWidth() const;

    void setWidth(uint32_t width);

    uint32_t getHeight() const;

    void setHeight(uint32_t height);

    double getRefreshRate() const;

    void setRefreshRate(double refreshRate);

    double getPreferredScale() const;

    void setPreferredScale(double preferredScale);

    const std::list<double> &getSupportedScales() const;

    void setSupportedScales(const std::list<double> &supportedScales);

};

struct CMutterMonitorSpec {
    std::string connector;
    std::string vendor;
    std::string product;
    std::string serial;
    std::string modeId;

    CMutterMonitorSpec(GVariant* variant);

    CMutterMonitorSpec();
};

class CMutterDisplayMonitor {
private:
    CMutterMonitorSpec* spec;
    std::list<CMutterMode*> modes;
public:
    CMutterDisplayMonitor();

    CMutterMonitorSpec *getSpec() const;

    ~CMutterDisplayMonitor();

    explicit CMutterDisplayMonitor(GVariant* variant);

};


class CMutterLogicalMonitor {
private:
    uint32_t x;
    uint32_t y;
    double scale;
    GVariant* transform;
    bool primary;
    std::list<CMutterMonitorSpec*> monitors;
public:
    CMutterLogicalMonitor();
    ~CMutterLogicalMonitor();

    explicit CMutterLogicalMonitor(GVariant* variant);

    uint32_t getX() const;

    void setX(uint32_t x);

    const std::list<CMutterMonitorSpec *> &getMonitors() const;

    uint32_t getY() const;

    void setY(uint32_t y);

    double getScale() const;

    void setScale(double scale);

    bool isPrimary() const;

    void setPrimary(bool primary);

    GVariant* ParseForApplyArgument();

};

class CMutterConfigDisplay {
private:
GVariant* serial;
GVariant* method;
CMutterLogicalMonitor* primaryMonitor;
std::list<CMutterLogicalMonitor*> logicalMonitors;
std::list<CMutterDisplayMonitor*> monitors;
void updateMonitorModeId();

public:

    CMutterConfigDisplay();

    ~CMutterConfigDisplay();
    explicit CMutterConfigDisplay(GVariant* variant);

    double GetScale();
    void SetScale(double scale);


    GVariant* CreateApplyArgument();

};


#endif //LSCALING_WAYLAND_MUTTER_CONFIG_DISPLAY_H
