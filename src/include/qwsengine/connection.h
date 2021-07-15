// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <QByteArray>
#include <QEnableSharedFromThis>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QtWebSockets/QWebSocket>

#include "qwsengine_export.h"

namespace QWsEngine {

class Handler;
class Middleware;
class ConnectionPrivate;

/**
 * @brief Each client connection is represented in a Connection instance, managed by ConnectionHandler.
 *
 * The Connection takes ownership of the provided QWebSocket object.
 */
class QWSENGINE_EXPORT Connection : public QObject, public QEnableSharedFromThis<QWsEngine::Connection> {
    Q_OBJECT

 public:
    explicit Connection(QWebSocket *webSocket, bool authenticated = false);
    Connection(QWebSocket *webSocket, Handler *handler, bool authenticated = false);
    virtual ~Connection();

    /**
     * @brief Set the root handler for all new messages
     */
    void     setHandler(Handler *handler);
    Handler *handler() const;

    QWebSocket *webSocket() const;

    bool isAuthenticated() const;
    void setAuthenticated(bool authenticated);

    virtual void sendErrorResponse(int statusCode, const QString &errorMsg = QString());
    virtual void sendAuthRequired();

    qint64 sendTextMessage(const QString &message);
    qint64 sendBinaryMessage(const QByteArray &data);

 public Q_SLOTS:  // NOLINT
    void processTextMessage(const QString &message);
    void processBinaryMessage(const QByteArray &message);

    void close(QWebSocketProtocol::CloseCode closeCode = QWebSocketProtocol::CloseCodeNormal,
               const QString &               reason = QString());

 private:
    QScopedPointer<ConnectionPrivate> const d;
    friend class ConnectionPrivate;
};

}  // namespace QWsEngine
