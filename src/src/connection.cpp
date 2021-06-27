// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#include <qwsengine/connection.h>
#include <qwsengine/handler.h>

#include "connection_p.h"
#include "wslogging_p.h"

namespace QWsEngine {

ConnectionPrivate::ConnectionPrivate(Connection *connection, QWebSocket *webSocket)
    : QObject(connection), socket(webSocket), handler(nullptr), authenticated(false), q(connection) {
    Q_ASSERT(webSocket);

    connect(webSocket, &QWebSocket::textMessageReceived, q, &Connection::processTextMessage);
    connect(webSocket, &QWebSocket::binaryMessageReceived, q, &Connection::processBinaryMessage);
}

Connection::Connection(QWebSocket *webSocket, bool authenticated) : d(new ConnectionPrivate(this, webSocket)) {
    setAuthenticated(authenticated);
}

Connection::Connection(QWebSocket *webSocket, Handler *handler, bool authenticated)
    : d(new ConnectionPrivate(this, webSocket)) {
    setHandler(handler);
    setAuthenticated(authenticated);
}

Connection::~Connection() {
    qCDebug(wsEngine) << "Connection deconstructor";
}

void Connection::setHandler(Handler *handler) {
    d->handler = handler;
}

Handler *Connection::handler() const {
    return d->handler;
}

QWebSocket *Connection::webSocket() const {
    return d->socket;
}

bool Connection::isAuthenticated() const {
    return d->authenticated;
}

void Connection::setAuthenticated(bool authenticated) {
    d->authenticated = authenticated;
}

void Connection::processTextMessage(const QString &message) {
    if (d->handler) {
        d->handler->routeTextMessage(this, message);
    } else {
        sendErrorResponse(500, "No message handler defined");
    }
}

void Connection::processBinaryMessage(const QByteArray &message) {
    if (d->handler) {
        d->handler->routeBinaryMessage(this, message);
    } else {
        sendErrorResponse(500, "No message handler defined");
    }
}

void Connection::close(QWebSocketProtocol::CloseCode closeCode, const QString &reason) {
    if (!d->socket) {
        return;
    }
    qCDebug(wsEngine) << "Close request for:" << d->socket->peerAddress().toString();
    d->socket->close(closeCode, reason);  // triggers onDisconnected
}

void Connection::sendErrorResponse(int statusCode, const QString &errorMsg) {
    qCDebug(wsEngine) << "Sending error response:" << statusCode;
    if (d->handler) {
        sendTextMessage(d->handler->errorResponseMsgTemplate().arg(statusCode).arg(errorMsg));
    } else {
        sendTextMessage(QString("{\"status_code\": %1}").arg(statusCode));
    }
}

void Connection::sendAuthRequired() {
    qCDebug(wsEngine) << "Sending auth required message";
    if (d->handler) {
        sendTextMessage(d->handler->authRequiredMsgTemplate());
    } else {
        sendTextMessage("{\"type\": \"auth_required\"}");
    }
}

qint64 Connection::sendTextMessage(const QString &message) {
    if (!d->socket || !d->socket->isValid()) {
        qCDebug(wsEngine) << "Invalid socket, cannot send message:" << message;
        return 0;
    }
    return d->socket->sendTextMessage(message.toUtf8());
}

qint64 Connection::sendBinaryMessage(const QByteArray &data) {
    if (!d->socket || !d->socket->isValid()) {
        qCDebug(wsEngine) << "Invalid socket, cannot send binary message";
        return 0;
    }
    return d->socket->sendBinaryMessage(data);
}

}  // namespace QWsEngine
