// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#include <qwsengine/msgauthconnectionhandler.h>

#include <QNetworkRequest>
#include <QTimer>

#include "connection_p.h"
#include "msgauthconnectionhandler_p.h"
#include "wslogging_p.h"

namespace QWsEngine {

MsgAuthConnectionHandlerPrivate::MsgAuthConnectionHandlerPrivate(MsgAuthConnectionHandler *handler)
    : QObject(handler), failedAuthClosesConnection(true), authTimeoutMs(30000), q(handler) {}

MsgAuthConnectionHandler::MsgAuthConnectionHandler(QObject *parent)
    : ConnectionHandler(parent), d(new MsgAuthConnectionHandlerPrivate(this)) {}

MsgAuthConnectionHandler::MsgAuthConnectionHandler(Handler *handler, QObject *parent)
    : ConnectionHandler(parent), d(new MsgAuthConnectionHandlerPrivate(this)) {
    setMessageHandler(handler);
}

MsgAuthConnectionHandler::~MsgAuthConnectionHandler() {
    qCDebug(wsEngine) << "MsgAuthConnectionHandler destructor";
}

QString MsgAuthConnectionHandler::name() const {
    return "MsgAuth";
}

void MsgAuthConnectionHandler::setFailedAuthClosesConnection(bool close) {
    d->failedAuthClosesConnection = close;
}

bool MsgAuthConnectionHandler::isFailedAuthClosesConnection() const {
    return d->failedAuthClosesConnection;
}

Connection *MsgAuthConnectionHandler::createConnection(QWebSocket *socket, bool authenticated) {
    Q_UNUSED(authenticated)

    auto conn = new Connection(socket, messageHandler(), false);
    conn->sendAuthRequired();

    QTimer::singleShot(d->authTimeoutMs, conn, [conn] {
        if (!conn->isAuthenticated()) {
            conn->sendErrorResponse(408, "Closing connection after authentication timeout");
            conn->close(QWebSocketProtocol::CloseCodePolicyViolated, "Authentication timeout");
        }
    });

    return conn;
}

}  // namespace QWsEngine
