// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/middleware.h>
#include <qwsengine/tokenauthenticator.h>

#include "qwsengine_export.h"

namespace QWsEngine {

class Connection;
class MsgAuthMiddlewarePrivate;

/**
 * @brief Pre-handler request message processor.
 *
 * Middleware sits between the connection handler and the final endpoint connection handler,
 * determining whether the message should be passed on to the handler.
 */
class QWSENGINE_EXPORT MsgAuthMiddleware : public Middleware {
    Q_OBJECT

 public:
    /**
     * @brief Base constructor for middleware
     */
    explicit MsgAuthMiddleware(const QString &msgName = "auth", const QString &tokenFieldName = "access_token",
                               QObject *parent = nullptr);

    virtual ~MsgAuthMiddleware();

    /**
     * @brief Name of the middleware for logging purposes
     */
    QString name() const override;

    void    setMsgName(const QString &msgName);
    QString msgName() const;
    void    setTokenFieldName(const QString &fieldName);
    QString tokenFieldName() const;

    void                setTokenAuthenticator(TokenAuthenticator *authenticator);
    TokenAuthenticator *tokenAuthenticator();

    /**
     * @brief Sets the error behaviour in case of failed header authentication.
     *
     * If set to true the socket is closed and the connection processing stops.
     * Defaults to true.
     */
    void setFailedAuthClosesSocket(bool close);
    bool isFailedAuthClosesSocket() const;

    /**
     * @brief Determine if request processing should continue
     *
     * This method is invoked when a new request comes in. If true is
     * returned, processing continues. Otherwise, it is assumed that an
     * appropriate error was written to the socket.
     */
    bool process(Connection *connection, const QString &msgName, const QVariant &message) override;

 private:
    MsgAuthMiddlewarePrivate *const d;
    friend class MsgAuthMiddlewarePrivate;
};

}  // namespace QWsEngine
