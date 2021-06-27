// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#include <qwsengine/authmiddleware.h>

#include "authmiddleware_p.h"
#include "connection_p.h"

namespace QWsEngine {

AuthMiddlewarePrivate::AuthMiddlewarePrivate(AuthMiddleware *middleware)
    : QObject(middleware), q(middleware) {}

AuthMiddleware::AuthMiddleware(QObject *parent) : Middleware(parent), d(new AuthMiddlewarePrivate(this)) {}

AuthMiddleware::~AuthMiddleware() {}

QString AuthMiddleware::name() const {
    return "AuthCheck";
}

bool AuthMiddleware::process(Connection *connection, const QString &msgName,
                                       const QVariant &message) {
    Q_UNUSED(message)

    if (!connection->isAuthenticated()) {
        connection->sendErrorResponse(401, "Authentication required");
        return false;
    }

    return true;
}

}  // namespace QWsEngine
