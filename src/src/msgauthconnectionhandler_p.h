// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/msgauthconnectionhandler.h>

#include <QList>
#include <QMap>
#include <QObject>

namespace QWsEngine {

class MsgAuthConnectionHandlerPrivate : public QObject {
    Q_OBJECT

 public:
    explicit MsgAuthConnectionHandlerPrivate(MsgAuthConnectionHandler *handler);

    bool failedAuthClosesConnection;
    int  authTimeoutMs;

 private:
    MsgAuthConnectionHandler *const q;
};

}  // namespace QWsEngine
