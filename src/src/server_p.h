// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/server.h>

#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QtWebSockets/QWebSocket>

namespace QWsEngine {

class Connection;
class ConnectionHandler;

class ServerPrivate : public QObject {
    Q_OBJECT

 public:
    explicit ServerPrivate(Server *wsServer);

    ConnectionHandler *handler;
    quint64            maxAllowedIncomingMessageSize;

    QMap<QWebSocket *, QSharedPointer<Connection>> connections;

 public Q_SLOTS:  // NOLINT
    void onNewConnection();
    void onServerClosed();
    void socketDisconnected();

 Q_SIGNALS:  // NOLINT
    void disconnectAllClients(QWebSocketProtocol::CloseCode closeCode = QWebSocketProtocol::CloseCodeNormal,
                              const QString &               reason = QString());

 private:
    Server *const q;
};

}  // namespace QWsEngine
