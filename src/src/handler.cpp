// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#include <qwsengine/connection.h>
#include <qwsengine/handler.h>
#include <qwsengine/middleware.h>

#include <QJsonDocument>
#include <QJsonParseError>

#include "handler_p.h"
#include "wslogging_p.h"

namespace QWsEngine {

HandlerPrivate::HandlerPrivate(Handler *handler) : QObject(handler), q(handler) {}

Handler::Handler(QObject *parent) : QObject(parent), d(new HandlerPrivate(this)) {}

Handler::~Handler() {}

void Handler::addMiddleware(Middleware *middleware) {
    d->middleware.append(middleware);
}

void Handler::addSubHandler(const QRegExp &msgNamePattern, Handler *handler) {
    d->subHandlers.append(SubHandler(msgNamePattern, handler));
}

void Handler::setErrorResponseMsgTemplate(const QString &messageTemplate) {
    d->errorTemplate = messageTemplate;
}

QString Handler::errorResponseMsgTemplate() const {
    return d->errorTemplate;
}

void Handler::setAuthRequiredMsgTemplate(const QString &messageTemplate) {
    d->authTemplate = messageTemplate;
}

QString Handler::authRequiredMsgTemplate() const {
    return d->authTemplate;
}

void Handler::routeTextMessage(Connection *connection, const QString &message) {
    qCDebug(wsEngine()) << "Converting WebSocket text message to JSON object";

    // TODO(zehnm) replace with MessageConverter

    QJsonParseError parseerror;
    QJsonDocument   doc = QJsonDocument::fromJson(message.toUtf8(), &parseerror);
    if (parseerror.error != QJsonParseError::NoError) {
        qCWarning(wsEngine) << "JSON error:" << parseerror.errorString();
        // TODO(zehnm) error handling codes. Try to extract request id with regex
        connection->sendErrorResponse(400, "Invalid json");
        return;
    }
    if (!doc.isObject()) {
        connection->sendErrorResponse(400, "Expected json object payload");
        return;
    }
    QJsonObject jsonObject = doc.object();
    // TODO(zehnm) refactor to user settable MessageNameExctractor class
    QVariant test = QVariant(jsonObject);
    route(connection, jsonObject.value("type").toString(), test);
}

void Handler::routeBinaryMessage(Connection *connection, const QByteArray &message) {
    // TODO(zehnm) add MessageConverter
    route(connection, "binary", message);
}

void Handler::route(Connection *connection, const QString &msgName, const QVariant &message) {
    // Run through each of the middleware
    foreach(Middleware *middleware, d->middleware) {
        if (!middleware->process(connection, msgName, message)) {
            return;
        }
    }

    // Check each of the sub-handlers for a match
    foreach(SubHandler subHandler, d->subHandlers) {
        if (subHandler.first.indexIn(msgName) != -1) {
            subHandler.second->route(connection, msgName, message);
            return;
        }
    }

    // If no match, invoke the process() method
    process(connection, msgName, message);
}

void Handler::process(Connection *connection, const QString &msgName, const QVariant &message) {
    Q_UNUSED(msgName)
    Q_UNUSED(message)
    if (connection->isAuthenticated()) {
        connection->sendErrorResponse(404, "Unknown message");
    } else {
        connection->sendErrorResponse(401, "Authentication required");
    }
}

}  // namespace QWsEngine
