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
 * @brief %Middleware for token based authentication in a JSON message.
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

    bool process(Connection *connection, const QString &msgName, const QVariant &message) override;

 private:
    MsgAuthMiddlewarePrivate *const d;
    friend class MsgAuthMiddlewarePrivate;
};

}  // namespace QWsEngine
