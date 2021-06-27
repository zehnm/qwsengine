// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/headerauthconnectionhandler.h>

#include <QList>
#include <QMap>
#include <QObject>

namespace QWsEngine {

class HeaderAuthConnectionHandlerPrivate : public QObject {
    Q_OBJECT

 public:
    explicit HeaderAuthConnectionHandlerPrivate(HeaderAuthConnectionHandler *handler, const QString &headerName);

    QString             tokenHeaderName;
    bool                failedAuthClosesSocket;
    bool                failedAuthCreatesConnection;
    TokenAuthenticator *tokenAuthenticator;

 private:
    HeaderAuthConnectionHandler *const q;
};

}  // namespace QWsEngine
