// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#include <qwsengine/connection.h>
#include <qwsengine/connectionhandler.h>
#include <qwsengine/connectionmiddleware.h>

#include "connectionhandler_p.h"
#include "wslogging_p.h"

namespace QWsEngine {

ConnectionHandlerPrivate::ConnectionHandlerPrivate(ConnectionHandler *connectionHandler)
    : QObject(connectionHandler), handler(nullptr), q(connectionHandler) {}

ConnectionHandler::ConnectionHandler(QObject *parent) : QObject(parent), d(new ConnectionHandlerPrivate(this)) {}

ConnectionHandler::ConnectionHandler(Handler *handler, QObject *parent)
    : QObject(parent), d(new ConnectionHandlerPrivate(this)) {
    d->handler = handler;
}

ConnectionHandler::~ConnectionHandler() {
    qCDebug(wsEngine) << "ConnectionHandler destructor";
}

QString ConnectionHandler::name() const {
    return "RootHandler";
}

void ConnectionHandler::addMiddleware(ConnectionMiddleware *middleware) {
    d->middleware.append(middleware);
}

void ConnectionHandler::addSubHandler(const QRegExp &pattern, ConnectionHandler *handler) {
    d->subHandlers.append(ConnSubHandler(pattern, handler));
}

QSharedPointer<Connection> ConnectionHandler::route(QWebSocket *socket, const QString &path) {
    qCDebug(wsEngine) << name() << ": Routing new" << path
                      << "connection through the middleware from:" << socket->peerAddress().toString()
                      << socket->peerPort();

    // Run through each of the middleware
    foreach(ConnectionMiddleware *middleware, d->middleware) {
        if (!middleware->process(socket)) {
            qCDebug(wsEngine) << "Middleware" << middleware->name()
                              << "stopped processing, closing socket from:" << socket->peerAddress().toString()
                              << socket->peerPort();
            socket->deleteLater();
            return nullptr;
        }
    }

    foreach(ConnSubHandler subHandler, d->subHandlers) {
        if (subHandler.first.indexIn(path) != -1) {
            qCDebug(wsEngine) << "Path regex(" << subHandler.first.pattern() << ") match on" << path
                              << "for sub-handler:" << subHandler.second->name();
            auto conn = subHandler.second->route(socket, path);
            if (conn || !socket->isValid()) {
                return conn;
            }
        }
    }

    // If no match, invoke the process() method
    return process(socket, path);
}

void ConnectionHandler::setMessageHandler(Handler *handler) {
    d->handler = handler;
}

Handler *ConnectionHandler::messageHandler() {
    return d->handler;
}

QSharedPointer<Connection> ConnectionHandler::process(QWebSocket *socket, const QString &path) {
    if (d->handler) {
        // simple connection without authentication: therefore set connection as authenticated to allow message
        // processing
        return createConnection(socket, true);
    }
    // The default response is to close the connection
    qCDebug(wsEngine) << "No connection processing available for" << path
                      << ", closing socket from:" << socket->peerAddress().toString() << socket->peerPort();
    // TODO(zehnm) send error msg?
    // TODO(zehnm) correct close code? Check WS specs!
    socket->close(QWebSocketProtocol::CloseCodePolicyViolated, "Invalid endpoint: " + path);
    socket->deleteLater();

    return nullptr;
}

QSharedPointer<Connection> ConnectionHandler::createConnection(QWebSocket *socket, bool authenticated) {
    return QSharedPointer<Connection>::create(socket, d->handler, authenticated);
}

}  // namespace QWsEngine
