// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#include <qwsengine/headerauthconnectionhandler.h>

#include <QNetworkRequest>

#include "connection_p.h"
#include "headerauthconnectionhandler_p.h"
#include "wslogging_p.h"

namespace QWsEngine {

HeaderAuthConnectionHandlerPrivate::HeaderAuthConnectionHandlerPrivate(HeaderAuthConnectionHandler *handler,
                                                                       const QString &              headerName)
    : QObject(handler),
      tokenHeaderName(headerName),
      failedAuthClosesSocket(true),
      failedAuthCreatesConnection(false),
      tokenAuthenticator(nullptr),
      q(handler) {}

HeaderAuthConnectionHandler::HeaderAuthConnectionHandler(const QString &tokenName, QObject *parent)
    : ConnectionHandler(parent), d(new HeaderAuthConnectionHandlerPrivate(this, tokenName)) {}

HeaderAuthConnectionHandler::HeaderAuthConnectionHandler(Handler *handler, const QString &tokenName, QObject *parent)
    : ConnectionHandler(parent), d(new HeaderAuthConnectionHandlerPrivate(this, tokenName)) {
    setMessageHandler(handler);
}

HeaderAuthConnectionHandler::~HeaderAuthConnectionHandler() {
    qCDebug(wsEngine) << "HeaderAuthConnectionHandler destructor";
}

QString HeaderAuthConnectionHandler::name() const {
    return "HeaderAuth";
}

void HeaderAuthConnectionHandler::setTokenHeaderName(const QString &headerName) {
    d->tokenHeaderName = headerName;
}

QString HeaderAuthConnectionHandler::tokenHeaderName() const {
    return d->tokenHeaderName;
}

void HeaderAuthConnectionHandler::setTokenAuthenticator(TokenAuthenticator *authenticator) {
    d->tokenAuthenticator = authenticator;
}

TokenAuthenticator *HeaderAuthConnectionHandler::tokenAuthenticator() {
    return d->tokenAuthenticator;
}

void HeaderAuthConnectionHandler::setFailedAuthClosesSocket(bool close) {
    d->failedAuthClosesSocket = close;
}

bool HeaderAuthConnectionHandler::isFailedAuthClosesSocket() const {
    return d->failedAuthClosesSocket;
}

void HeaderAuthConnectionHandler::setFailedAuthCreatesConnection(bool create) {
    d->failedAuthCreatesConnection = create;
}

bool HeaderAuthConnectionHandler::isFailedAuthCreatesConnection() const {
    return d->failedAuthCreatesConnection;
}

QSharedPointer<Connection> HeaderAuthConnectionHandler::process(QWebSocket *socket, const QString &path) {
    auto networkRequest = socket->request();
    auto headerName = d->tokenHeaderName.toUtf8();
    bool authenticated = false;

    if (networkRequest.hasRawHeader(headerName)) {
        QString token = networkRequest.rawHeader(headerName);

        if (d->tokenAuthenticator) {
            authenticated = d->tokenAuthenticator->authenticate(path, token);
        }

    } else {
        if (d->failedAuthClosesSocket) {
            qCDebug(wsEngine) << "Missing required authorization header" << d->tokenHeaderName << "for endpoint" << path
                              << "from:" << socket->peerAddress().toString() << socket->peerPort();
        }
    }

    if (!authenticated && d->failedAuthClosesSocket) {
        qCDebug(wsEngine) << "Failed authentication for endpoint" << path
                          << ", closing socket from:" << socket->peerAddress().toString() << socket->peerPort();
        // TODO(zehnm) correct close code? Check WS specs!
        socket->close(QWebSocketProtocol::CloseCodePolicyViolated, "Authentication failed");
        socket->deleteLater();
        return nullptr;
    }

    if (!authenticated && !d->failedAuthCreatesConnection) {
        return nullptr;
    }

    return createConnection(socket, authenticated);
}

}  // namespace QWsEngine
