// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#include <qwsengine/connectionhandler.h>

#include <QDebug>

#include "connection_p.h"
#include "server_p.h"
#include "wslogging_p.h"

namespace QWsEngine {

ServerPrivate::ServerPrivate(Server *httpServer)
    : QObject(httpServer), handler(nullptr), maxAllowedIncomingMessageSize(0), q(httpServer) {
    connect(q, &QWebSocketServer::newConnection, this, &ServerPrivate::onNewConnection);
}

void ServerPrivate::onNewConnection() {
    QWebSocket *socket = q->nextPendingConnection();
    if (socket == nullptr) {
        return;  // should never happen, but safety first!
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    if (maxAllowedIncomingMessageSize > 0) {
        socket->setMaxAllowedIncomingMessageSize(maxAllowedIncomingMessageSize);
    }
#endif

    QString path = socket->requestUrl().path();

    if (handler) {
        auto conn = handler->route(socket, path);
        if (conn) {
            qCDebug(wsEngine) << "Created new" << path << "client connection from:" << socket->peerAddress().toString()
                              << socket->peerPort();
            connect(this, &ServerPrivate::disconnectAllClients, conn, &Connection::close);

            connect(socket, &QWebSocket::disconnected, this, &ServerPrivate::socketDisconnected);
            connections.insert(socket, conn);
        }
    } else {
        qCWarning(wsEngine) << "No handler defined, closing connection:" << socket->peerAddress().toString() << path;
        // TODO(zehnm) correct close code? Check WS specs!
        socket->close(QWebSocketProtocol::CloseCodePolicyViolated, "Internal server error");
        socket->deleteLater();
    }
}

void ServerPrivate::socketDisconnected() {
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
    if (socket) {
        qCDebug(wsEngine) << "Client disconnected:" << socket->peerAddress().toString() << socket->peerPort();
        if (connections.contains(socket)) {
            qCDebug(wsEngine) << "Deleting connection";
            auto *conn = connections.take(socket);
            //            emit clientDisconnected(conn->clientInfo());
            conn->deleteLater();
        }
        socket->close();  // TODO(zehnm) really required?
        socket->deleteLater();
    }
}

void ServerPrivate::onServerClosed() {
    // TODO(zehnm) flag? There might be use cases where the server must be closed
    // but the established connections have to stay alive
    emit disconnectAllClients(QWebSocketProtocol::CloseCodeGoingAway);
}

Server::Server(const QString &serverName, SslMode secureMode, QObject *parent)
    : QWebSocketServer(serverName, secureMode, parent), d(new ServerPrivate(this)) {}

Server::Server(ConnectionHandler *handler, const QString &serverName, SslMode secureMode, QObject *parent)
    : QWebSocketServer(serverName, secureMode, parent), d(new ServerPrivate(this)) {
    setHandler(handler);
}

Server::~Server() {}

void Server::setHandler(ConnectionHandler *handler) {
    d->handler = handler;
}

void Server::setMaxAllowedIncomingMessageSize(quint64 maxAllowedIncomingMessageSize) {
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    qCWarning(wsEngine) << "setMaxAllowedIncomingMessageSize requires Qt 5.15";
#endif
    d->maxAllowedIncomingMessageSize = maxAllowedIncomingMessageSize;
}

}  // namespace QWsEngine
