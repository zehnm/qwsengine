// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <QtWebSockets/QWebSocketServer>

#include "qwsengine_export.h"

namespace QWsEngine {

class ConnectionHandler;
class ServerPrivate;

class QWSENGINE_EXPORT Server : public QWebSocketServer {
    Q_OBJECT

 public:
    /**
     * @brief Create a WebSocket server
     */
    explicit Server(const QString &serverName = QString(), SslMode secureMode = QWebSocketServer::NonSecureMode,
                    QObject *parent = nullptr);

    /**
     * @brief Create a WebSocket server with the specified connection handler
     */
    Server(ConnectionHandler *handler, const QString &serverName = QString(),
           SslMode secureMode = QWebSocketServer::NonSecureMode, QObject *parent = nullptr);

    virtual ~Server();

    /**
     * @brief Set the root connection handler for all new requests
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
