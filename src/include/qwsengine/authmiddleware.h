// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/middleware.h>

#include "qwsengine_export.h"

namespace QWsEngine {

class Connection;
class AuthMiddlewarePrivate;

/**
 * @brief %Middleware for checking if the connection is authenticated.
 *
 * The authentication middleware blocks all requests as long as the
 * corresponding connection hasn't been authenticated yet.
 * The authentication itself is not part of the middleware.
 */
class QWSENGINE_EXPORT AuthMiddleware : public Middleware {
    Q_OBJECT

 public:
    /**
     * @brief Base constructor for the middleware
     */
    explicit AuthMiddleware(QObject *parent = nullptr);

    virtual ~AuthMiddleware();

    /**
     * @brief Name of the middleware for logging purposes
     */
    QString name() const override;

    /**
     * @brief Check if the connection has been authenticated.
     *
     * An error message with code 401 "Authentication required" is sent to the
     * client if the corresponding %Connection is not yet authenticated.
     */
    bool process(Connection *connection, const QString &msgName, const QVariant &message) override;

 private:
    AuthMiddlewarePrivate *const d;
    friend class AuthMiddlewarePrivate;
};

}  // namespace QWsEngine
