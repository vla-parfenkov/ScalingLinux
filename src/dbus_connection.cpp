//
// Created by vladislav on 01.09.18.
//

#include <dbus_connection.h>

#include "dbus_connection.h"


CDBusConnection::CDBusConnection() : connection(nullptr), error(nullptr) {
    connection = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, &error);
    if(error || !connection) {
        throw std::logic_error(error->message);
    }
    g_clear_error (&error);
}



CDBusConnection::~CDBusConnection() {
    if (connection != nullptr) {
        g_object_unref(connection);
        connection = nullptr;
    }
}

GVariant *CDBusConnection::CallMethod(const std::string bus, const std::string object, const std::string interface,
                                      const std::string method, GVariant *param) {
    GVariant* reply = nullptr;
    reply = g_dbus_connection_call_sync(connection, bus.c_str(), object.c_str(), interface.c_str(), method.c_str(),
            param, NULL,  G_DBUS_CALL_FLAGS_NO_AUTO_START, -1, NULL, &error);
    if (error) {
        throw std::logic_error(error->message);
    }
    return reply;
}

bool CDBusConnection::IsInterface(const std::string &interface) {
    return static_cast<bool>(g_dbus_is_interface_name(interface.c_str()));
}
