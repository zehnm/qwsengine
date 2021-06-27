// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/msgauthmiddleware.h>

#include <QObject>

namespace QWsEngine {

class MsgAuthMiddlewarePrivate : public QObject {
    Q_OBJECT

 public:
    explicit MsgAuthMiddlewarePrivate(MsgAuthMiddleware *handler);

    QString             msgName;
    QString             tokenFieldName;
    bool                failedAuthClosesSocket;
    TokenAuthenticator *authenticator;

 private:
    MsgAuthMiddleware *const q;
};

}  // namespace QWsEngine
