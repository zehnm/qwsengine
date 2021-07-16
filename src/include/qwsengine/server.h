// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <QtWebSockets/QWebSocketServer>

#include "qwsengine_export.h"

namespace QWsEngine {

class ConnectionHandler;
class ServerPrivate;

/**
 * @brief WebSocket server extending Qt's QWebSocketServer for convenient JSON payload handling in text messages.
 *
 * The QWebSocket instance retrieved for new client connections is managed by QWsEngine::Connection, which in turn is
 * passed around with a QSharedPointer. The server keeps track of all open QWebSocket instances. The QWebSocket's
 * disconnect() signal is used to ensure that the connection and socket objects are deleted when the client disconnects.
 */
class QWSENGINE_EXPORT Server : public QWebSocketServer {
    Q_OBJECT

 public:
    /**
     * @brief Constructs a new WebSocket server.
     *
     * Simply calls the corresponding QWebSocketServer constructor.
     * The method setHandler must be called separately to initialize and use the server.
     */
    explicit Server(const QString &serverName = QString(), SslMode secureMode = QWebSocketServer::NonSecureMode,
                    QObject *parent = nullptr);

    /**
     * @brief Constructs a new WebSocket server with the specified connection handler.
     */
    Server(ConnectionHandler *handler, const QString &serverName = QString(),
           SslMode secureMode = QWebSocketServer::NonSecureMode, QObject *parent = nullptr);

    virtual ~Server();

    /**
     * @brief Set the root connection handler for all new requests.
     */
    void setHandler(ConnectionHandler *handler);

    /**
     * @brief Sets the maximum allowed size of an incoming websocket message to maxAllowedIncomingMessageSize.
     *
     * If an incoming message exceeds this limit, the peer gets disconnected. The accepted range is between 0 and
     * QWebSocket::maxIncomingMessageSize(), default is QWebSocket::maxIncomingMessageSize().
     */
    void setMaxAllowedIncomingMessageSize(quint64 maxAllowedIncomingMessageSize);

 private:
    ServerPrivate *const d;
    friend class ServerPrivate;
};

}  // namespace QWsEngine
