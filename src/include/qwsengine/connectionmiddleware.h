// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <QObject>
#include <QtWebSockets/QWebSocket>

#include "qwsengine_export.h"

namespace QWsEngine {

class Socket;

/**
 * @brief WebSocket connection processor.
 *
 * ConnectionMiddleware sits between the server and the final endpoint connection handler,
 * determining whether the connection should be passed on to the handler.
 *
 * Possible use cases:
 * - filter clients based on IP address, e.g. only allow local subnet
 * - verify url pathes
 * - verify a required header is set. Attention: setting headers is no longer possible in most browsers!
 */
class QWSENGINE_EXPORT ConnectionMiddleware : public QObject {
    Q_OBJECT

 public:
    /**
     * @brief Base constructor for connection middleware
     */
    explicit ConnectionMiddleware(QObject *parent = nullptr) : QObject(parent) {}

    /**
     * @brief Name of the middleware for logging purposes
     */
    virtual QString name() const = 0;

    /**
     * @brief Determine if request processing should continue
     *
     * This method is invoked when a new connection is established. If true is
     * returned, processing continues. Otherwise, it is assumed that an
     * appropriate error was written to the socket and that the socket has been closed.
     * The socket will be deleted by the [ConnectionHandler](@ref QWsEngine::ConnectionHandler).
     */
    virtual bool process(QWebSocket *socket) = 0;
};

}  // namespace QWsEngine
