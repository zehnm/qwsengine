// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#include <qwsengine/msgauthmiddleware.h>

#include <QJsonObject>

#include "connection_p.h"
#include "handler_p.h"
#include "msgauthmiddleware_p.h"
#include "wslogging_p.h"

namespace QWsEngine {

MsgAuthMiddlewarePrivate::MsgAuthMiddlewarePrivate(MsgAuthMiddleware *handler)
    : QObject(handler), failedAuthClosesSocket(true), authenticator(nullptr), q(handler) {}

MsgAuthMiddleware::MsgAuthMiddleware(const QString &msgName, const QString &tokenFieldName, QObject *parent)
    : Middleware(parent), d(new MsgAuthMiddlewarePrivate(this)) {
    setMsgName(msgName);
    setTokenFieldName(tokenFieldName);
}

MsgAuthMiddleware::~MsgAuthMiddleware() {}

QString MsgAuthMiddleware::name() const {
    return "MsgAuth";
}

void MsgAuthMiddleware::setMsgName(const QString &msgName) {
    d->msgName = msgName;
}

QString MsgAuthMiddleware::msgName() const {
    return d->msgName;
}

void MsgAuthMiddleware::setTokenFieldName(const QString &fieldName) {
    d->tokenFieldName = fieldName;
}

QString MsgAuthMiddleware::tokenFieldName() const {
    return d->tokenFieldName;
}

void MsgAuthMiddleware::setTokenAuthenticator(TokenAuthenticator *authenticator) {
    d->authenticator = authenticator;
}

TokenAuthenticator *MsgAuthMiddleware::tokenAuthenticator() {
    return d->authenticator;
}

void MsgAuthMiddleware::setFailedAuthClosesSocket(bool close) {
    d->failedAuthClosesSocket = close;
}

bool MsgAuthMiddleware::isFailedAuthClosesSocket() const {
    return d->failedAuthClosesSocket;
}

bool MsgAuthMiddleware::process(QSharedPointer<Connection> connection, const QString &msgName,
                                const QVariant &message) {
    if (message.type() != QMetaType::QJsonObject) {
        // not a json object message, continue.
        qCWarning(wsEngine()) << "Expected QJsonObject message but got:" << message.typeName();
        connection->sendErrorResponse(400, "Expected json object payload");
        return true;
    }

    auto jsonMsg = message.toJsonObject();

    // check optional message identification field
    if (!d->msgName.isEmpty() && msgName.compare(d->msgName) != 0) {
        // no authentication message, continue
        return true;
    }

    // check token
    if (jsonMsg.contains(d->tokenFieldName)) {
        QString token = jsonMsg.value(d->tokenFieldName).toString();

        bool authenticated = false;
        if (d->authenticator && connection->webSocket()) {
            authenticated = d->authenticator->authenticate(connection->webSocket()->requestUrl().path(), token);
        }

        connection->setAuthenticated(authenticated);

        if (!authenticated && d->failedAuthClosesSocket) {
            connection->sendErrorResponse(403, "Authentication failed");
            // TODO(zehnm) correct WS close code? Check WS specs!
            connection->close(QWebSocketProtocol::CloseCodePolicyViolated, "Invalid credentials");
        }
        return false;
    }

    // continue processing
    return true;
}

}  // namespace QWsEngine
