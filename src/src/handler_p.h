// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/handler.h>

#include <QList>
#include <QObject>

namespace QWsEngine {

typedef QPair<QRegExp, Handler *> SubHandler;

class HandlerPrivate : public QObject {
    Q_OBJECT

 public:
    explicit HandlerPrivate(Handler *handler);

    QList<Middleware *> middleware;
    QList<SubHandler>   subHandlers;
    QString             errorTemplate =
        "{\"type\": \"result\", \"success\": false, \"error\": {\"code\": %1, \"message\": \"%2\"}}";
    QString authTemplate = "{\"type\": \"auth_required\"}";

 private:
    Handler *const q;
};

}  // namespace QWsEngine
