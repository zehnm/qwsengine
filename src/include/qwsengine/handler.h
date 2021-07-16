// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <QByteArray>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QVariant>
#include <QtWebSockets/QWebSocket>

#include "qwsengine_export.h"

namespace QWsEngine {

class Connection;
class Middleware;
class HandlerPrivate;

/**
 * @brief Base class for WebSocket message handlers
 *
 * When a message is received by a [ConnectionHandler](@ref QWsEngine::ConnectionHandler), it
 * invokes the route() method of the root handler which is used to determine
 * what happens to the message. All WebSocket handlers derive from this class
 * should override the protected process() method in order to process the message.
 */
class QWSENGINE_EXPORT Handler : public QObject {
    Q_OBJECT

 public:
    /**
     * @brief Base constructor for a message handler
     */
    explicit Handler(QObject *parent = nullptr);

    virtual ~Handler();

    /**
     * @brief Add mesasge middleware to the handler
     */
    void addMiddleware(Middleware *middleware);

    /**
     * @brief Add a handler for a specific message name pattern
     *
     * The pattern and handler will be added to an internal list that will be
     * used when the route() method is invoked to determine whether the
     * request matches any patterns. The order of the list is preserved.
     */
    void addSubHandler(const QRegExp &msgNamePattern, Handler *handler);

    // Message templates are stored in the handler to avoid duplicating them in each connection.
    // Otherwise they would have to be a const string without customization option or through an ugly singleton.

    void    setErrorResponseMsgTemplate(const QString &messageTemplate);
    QString errorResponseMsgTemplate() const;
    void    setAuthRequiredMsgTemplate(const QString &messageTemplate);
    QString authRequiredMsgTemplate() const;

    virtual void routeTextMessage(QSharedPointer<Connection> connection, const QString &message);
    virtual void routeBinaryMessage(QSharedPointer<Connection> connection, const QByteArray &message);

 protected:
    /**
     * @brief Route an incoming message
     */
    void route(QSharedPointer<Connection> connection, const QString &msgName, const QVariant &message);

    /**
     * @brief Process a message
     *
     * This method should process the message either by fulfilling it, or
     * writing an error to the socket.
     */
    virtual void process(QSharedPointer<Connection> connection, const QString &msgName, const QVariant &message);

 private:
    HandlerPrivate *const d;
    friend class HandlerPrivate;
};

}  // namespace QWsEngine
