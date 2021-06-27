// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <QObject>

#include "qwsengine_export.h"

namespace QWsEngine {

class QWSENGINE_EXPORT TokenAuthenticator : public QObject {
    Q_OBJECT

 public:
    explicit TokenAuthenticator(QObject *parent = nullptr) : QObject(parent) {}

    virtual bool authenticate(const QString &path, const QString &token) = 0;
};

}  // namespace QWsEngine
