// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/connectionhandler.h>
#include <qwsengine/handler.h>

#include <QList>
#include <QObject>
#include <QPair>
#include <QRegExp>

namespace QWsEngine {

typedef QPair<QRegExp, ConnectionHandler *> ConnSubHandler;

class ConnectionHandlerPrivate : public QObject {
    Q_OBJECT

 public:
    explicit ConnectionHandlerPrivate(ConnectionHandler *connectionHandler);

    QList<ConnectionMiddleware *> middleware;
    QList<ConnSubHandler>         subHandlers;
    Handler *                     handler;

 private:
    ConnectionHandler *const q;
};

}  // namespace QWsEngine
