// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/connection.h>

#include <QList>
#include <QObject>
#include <QtWebSockets/QWebSocket>

namespace QWsEngine {

class ConnectionPrivate : public QObject {
    Q_OBJECT

 public:
    explicit ConnectionPrivate(Connection *connection, QWebSocket *socket);

    QList<Middleware *> middleware;

    QWebSocket *socket;
    Handler *   handler;
    bool        authenticated;

 private:
    Connection *const q;
};

}  // namespace QWsEngine
