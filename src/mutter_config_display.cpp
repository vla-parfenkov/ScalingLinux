//
// Created by vladislav on 02.09.18.
//

#include "mutter_config_display.h"


uint32_t CMutterMode::getWidth() const {
    return width;
}

void CMutterMode::setWidth(uint32_t width) {
    CMutterMode::width = width;
}

uint32_t CMutterMode::getHeight() const {
    return height;
}

void CMutterMode::setHeight(uint32_t height) {
    CMutterMode::height = height;
}

double CMutterMode::getRefreshRate() const {
    return refreshRate;
}

void CMutterMode::setRefreshRate(double refreshRate) {
    CMutterMode::refreshRate = refreshRate;
}

double CMutterMode::getPreferredScale() const {
    return preferredScale;
}

void CMutterMode::setPreferredScale(double preferredScale) {
    CMutterMode::preferredScale = preferredScale;
}

const std::list<double> &CMutterMode::getSupportedScales() const {
    return supportedScales;
}

void CMutterMode::setSupportedScales(const std::list<double> &supportedScales) {
    CMutterMode::supportedScales = supportedScales;
}

CMutterMode::CMutterMode()  {

}

CMutterMode::~CMutterMode() {
}

CMutterMode::CMutterMode(GVariant *variant) {
    char* id;
    GVariantIter *scales = nullptr;
    double scale;
    GVariantIter* prop;

    g_variant_get(variant, format_mutter_interface::mode.c_str(), &id, &width, &height, &refreshRate,
            &preferredScale, &scales, &prop);
    this->id.append(id);
    parseProp(prop);
    std::string scalesStr =  "d";
    while (g_variant_iter_next(scales, scalesStr.c_str(), &scale)) {
        this->supportedScales.push_back(scale);
    }

    g_variant_iter_free(scales);

}

void CMutterMode::parseProp(GVariantIter *prop) {
        const char *key;
        GVariant *value;
        isCurrent = false;

        while (g_variant_iter_next (prop, "{&sv}", &key, &value)) {
            if (!g_str_equal (key, "is-current")) {
                g_variant_unref (value);
                isCurrent = true;
                continue;
            }
        }

}

std::string CMutterMode::getId(){
    return id;
}

bool CMutterMode::isIsCurrent() const {
    return isCurrent;
}

CMutterDisplayMonitor::CMutterDisplayMonitor() {}

CMutterDisplayMonitor::~CMutterDisplayMonitor() {
    delete spec;
    for(auto& item : modes)
    {
        delete item;
    }

}


CMutterDisplayMonitor::CMutterDisplayMonitor(GVariant *variant) {
    GVariantIter *modes = nullptr;
    GVariant* modeVariant = nullptr;
    spec = new CMutterMonitorSpec();
    char* connector;
    char* vendor;
    char* product;
    char* serial;


    g_variant_get(variant, format_mutter_interface::monitor.c_str(), &connector, &vendor,
            &product, &serial, &modes, NULL);

    spec->connector.append(connector);
    spec->vendor.append(vendor);
    spec->product.append(product);
    spec->serial.append(serial);

    std::string modesStr =  "@" + format_mutter_interface::mode;
    while (g_variant_iter_next(modes, modesStr.c_str(), &modeVariant)) {
        auto mode = new CMutterMode(modeVariant);
        if(mode->isIsCurrent()) {
            spec->modeId = mode->getId();
        }
        this->modes.push_back(mode);
        g_variant_unref (modeVariant);
    }

    g_variant_iter_free(modes);
}

CMutterMonitorSpec *CMutterDisplayMonitor::getSpec() const {
    return spec;
}


CMutterLogicalMonitor::CMutterLogicalMonitor() {}

uint32_t CMutterLogicalMonitor::getX() const {
    return x;
}

void CMutterLogicalMonitor::setX(uint32_t x) {
    CMutterLogicalMonitor::x = x;
}

uint32_t CMutterLogicalMonitor::getY() const {
    return y;
}

void CMutterLogicalMonitor::setY(uint32_t y) {
    CMutterLogicalMonitor::y = y;
}

double CMutterLogicalMonitor::getScale() const {
    return scale;
}

void CMutterLogicalMonitor::setScale(double scale) {
    CMutterLogicalMonitor::scale = scale;
}


bool CMutterLogicalMonitor::isPrimary() const {
    return primary;
}

void CMutterLogicalMonitor::setPrimary(bool primary) {
    CMutterLogicalMonitor::primary = primary;
}


CMutterLogicalMonitor::~CMutterLogicalMonitor() {
    for(auto& item : monitors)
    {
        delete item;
    }
}

CMutterLogicalMonitor::CMutterLogicalMonitor(GVariant *variant) {
    GVariantIter *monitors = nullptr;
    GVariant* monitorVariant = nullptr;

    g_variant_get(variant, format_mutter_interface::logical_monitor.c_str(), &x, &y, &scale, &transform,
            &primary, &monitors, NULL);


    std::string monitorsStr =  "@" + format_mutter_interface::monitor_spec;
    while (g_variant_iter_next(monitors, monitorsStr.c_str(), &monitorVariant)) {
        this->monitors.push_back(new CMutterMonitorSpec(monitorVariant));
        g_variant_unref (monitorVariant);
    }

    g_variant_iter_free(monitors);
}

GVariant *CMutterLogicalMonitor::ParseForApplyArgument() {
    GVariantBuilder builder;

    g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);
    for(auto& monitor : monitors)
    {
        GVariantBuilder props_builder;

        g_variant_builder_init (&props_builder, G_VARIANT_TYPE ("a{sv}"));
        g_variant_builder_add (&props_builder, "{sv}",
                               "underscanning",
                               g_variant_new_boolean (true));
        g_variant_builder_add (&builder, "(ss@*)",
                               monitor->connector.c_str(),
                               monitor->modeId.c_str(),
                               g_variant_builder_end (&props_builder));
    }


    return g_variant_new ("(iidub@*)",
                          x,
                          y,
                          scale,
                          transform,
                          primary,
                          g_variant_builder_end (&builder));
}

const std::list<CMutterMonitorSpec *> &CMutterLogicalMonitor::getMonitors() const {
    return monitors;
}

CMutterConfigDisplay::~CMutterConfigDisplay() {
    for(auto& item : monitors)
    {
        delete item;
    }
    for(auto& item : logicalMonitors)
    {
        delete item;
    }
    primaryMonitor = nullptr;
}

CMutterConfigDisplay::CMutterConfigDisplay() {

}

CMutterConfigDisplay::CMutterConfigDisplay(GVariant *variant) {
    GVariantIter *logicalMonitors = nullptr;
    GVariantIter *monitors = nullptr;
    GVariant *logicalMonitorVariant = nullptr;
    GVariant* monitorVariant = nullptr;

    g_variant_get(variant, format_mutter_interface::current_state.c_str(), &serial, &monitors,
            &logicalMonitors, NULL);

    std::string logicalMonitorsStr = "@" + format_mutter_interface::logical_monitor;

    while (g_variant_iter_next (logicalMonitors, logicalMonitorsStr.c_str(),
                                &logicalMonitorVariant)) {
        auto logicalMonitor = new CMutterLogicalMonitor(logicalMonitorVariant);
        this->logicalMonitors.push_back(logicalMonitor);
        if(logicalMonitor->isPrimary()) {
            primaryMonitor = logicalMonitor;
        }
        g_variant_unref (logicalMonitorVariant);
    }

    std::string monitorsStr =  "@" + format_mutter_interface::monitor;
    while (g_variant_iter_next(monitors, monitorsStr.c_str(), &monitorVariant)) {
        this->monitors.push_back(new CMutterDisplayMonitor(monitorVariant));
        g_variant_unref (monitorVariant);
    }

    updateMonitorModeId();
    g_variant_iter_free(logicalMonitors);
    g_variant_iter_free(monitors);

}

double CMutterConfigDisplay::GetScale() {
    return primaryMonitor->getScale();
}

void CMutterConfigDisplay::SetScale(double scale) {
    primaryMonitor->setScale(scale);
}

GVariant *CMutterConfigDisplay::CreateApplyArgument() {
    const uint8_t method_number = 2;
    GVariantBuilder props_builder;
    g_variant_builder_init (&props_builder, G_VARIANT_TYPE ("a{sv}"));
    g_variant_builder_add (&props_builder, "{sv}", "layout-mode",
            g_variant_new_uint32 (format_mutter_interface::dpy_layout_mode_logical));


    method = g_variant_new_uint32(method_number);

    GVariantBuilder builder;

    g_variant_builder_init (&builder, G_VARIANT_TYPE_ARRAY);
    g_variant_builder_add (&builder, "@*", primaryMonitor->ParseForApplyArgument());


    return g_variant_new ("(uu@*@*)",
                          serial,
                          method,
                          g_variant_builder_end(&builder),
                          g_variant_builder_end (&props_builder));
}


void CMutterConfigDisplay::updateMonitorModeId() {
        for (auto &logicalMonitor : logicalMonitors) {
            for (auto &monitorSpec : logicalMonitor->getMonitors()) {
                for (auto &monitor : monitors) {
                    if(monitor->getSpec()->connector == monitorSpec->connector) {
                        monitorSpec->modeId = monitor->getSpec()->modeId;
                    }
                }
            }
        }

}

CMutterMonitorSpec::CMutterMonitorSpec(GVariant *variant) {
    char* connector;
    char* vendor;
    char* product;
    char* serial;


    g_variant_get(variant, format_mutter_interface::monitor_spec.c_str(), &connector, &vendor, &product, &serial);;

    this->connector.append(connector);
    this->vendor.append(vendor);
    this->product.append(product);
    this->serial.append(serial);

}

CMutterMonitorSpec::CMutterMonitorSpec()  {}
