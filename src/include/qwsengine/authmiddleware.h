// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/middleware.h>

#include "qwsengine_export.h"

namespace QWsEngine {

class Connection;
class AuthMiddlewarePrivate;

/**
 * @brief Pre-handler request message processor.
 *
 * Middleware sits between the connection handler and the final endpoint connection handler,
 * determining whether the message should be passed on to the handler.
 */
class QWSENGINE_EXPORT AuthMiddleware : public Middleware {
    Q_OBJECT

 public:
    /**
     * @brief Base constructor for middleware
     */
    explicit AuthMiddleware(QObject *parent = nullptr);

    virtual ~AuthMiddleware();

    /**
     * @brief Name of the middleware for logging purposes
     */
    QString name() const override;

    /**
     * @brief Determine if request processing should continue
     *
     * This method is invoked when a new request comes in. If true is
     * returned, processing continues. Otherwise, it is assumed that an
     * appropriate error was written to the socket.
     */
    bool process(Connection *connection, const QString &msgName, const QVariant &message) override;

 private:
    AuthMiddlewarePrivate *const d;
    friend class AuthMiddlewarePrivate;
};

}  // namespace QWsEngine
