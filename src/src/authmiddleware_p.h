// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/authmiddleware.h>

#include <QObject>

namespace QWsEngine {

class AuthMiddlewarePrivate : public QObject {
    Q_OBJECT

 public:
    explicit AuthMiddlewarePrivate(AuthMiddleware *handler);

 private:
    AuthMiddleware *const q;
};

}  // namespace QWsEngine
