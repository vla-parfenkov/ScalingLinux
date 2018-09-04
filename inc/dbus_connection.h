//
// Created by vladislav on 01.09.18.
//

#ifndef LSCALING_DBUS_CONNECTION_H
#define LSCALING_DBUS_CONNECTION_H

#include <stdexcept>
#include <queue>
#include <gio/gio.h>

class CDBusConnection {
private:
    GDBusConnection* connection;
    GError* error;

public:
    CDBusConnection();
    ~CDBusConnection();


    static bool IsInterface(const std::string& interface);
    GVariant* CallMethod(const std::string bus, const std::string object, const std::string interface,
                         const std::string method, GVariant* param);

};


#endif //LSCALING_DBUS_CONNECTION_H
