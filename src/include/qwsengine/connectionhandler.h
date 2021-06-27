// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <QObject>
#include <QRegExp>
#include <QtWebSockets/QWebSocket>

#include "qwsengine_export.h"

namespace QWsEngine {

class Connection;
class ConnectionMiddleware;
class ConnectionHandlerPrivate;
class Handler;

/**
 * @brief Base class for WebSocket connection handlers
 *
 * When a new connection is established by a [Server](@ref WsEngine::Server), it
 * invokes the route() method of the root handler which is used to determine
 * what happens to the connection request. All WebSocket handlers derive from this class
 * should override the protected process() method in order to process the request.
 */
class QWSENGINE_EXPORT ConnectionHandler : public QObject {
    Q_OBJECT

 public:
    /**
     * @brief Base constructor for a handler
     */
    explicit ConnectionHandler(QObject *parent = 0);
    explicit ConnectionHandler(Handler *handler, QObject *parent = 0);
    virtual ~ConnectionHandler();

    /**
     * @brief Name of the connection handler for logging purposes
     */
    virtual QString name() const;

    /**
     * @brief Add middleware to the handler
     */
    void addMiddleware(ConnectionMiddleware *middleware);

    /**
     * @brief Add a handler
     *
     * The handler will be added to an internal list that will be
     * used when the route() method is invoked, after the middleware has been processed.
     * If the sub handler returns a Connection, further processing is stopped and the connection returned to the caller.
     * The order of the list is preserved.
     */
    void addSubHandler(const QRegExp &pattern, ConnectionHandler *handler);

    /**
     * @brief Route an incoming connection
     */
    Connection *route(QWebSocket *socket, const QString &path);

    /**
     * @brief Set the root message handler for the created connections
     */
    void     setMessageHandler(Handler *handler);
    Handler *messageHandler();

 protected:
    /**
     * @brief Process a new connection
     *
     * This method should process the connection either by fulfilling it and returning a new [Connection](@ref
     * WsEngine::Connection), or if the connection is rejected returning nullptr and deleting the socket after writing
     * an error to the socket and closing it.
     */
    virtual Connection *process(QWebSocket *socket, const QString &path);

    /**
     * @brief Connection creation hook.
     *
     * Override to create custom connections.
     */
    virtual Connection *createConnection(QWebSocket *socket, bool authenticated = false);

 private:
    ConnectionHandlerPrivate *const d;
    friend class ConnectionHandlerPrivate;
};

}  // namespace QWsEngine
