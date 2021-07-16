// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QVariant>
#include <QtWebSockets/QWebSocket>

#include "qwsengine_export.h"

namespace QWsEngine {

class Connection;

/**
 * @brief Pre-handler request message processor.
 *
 * Middleware sits between the connection handler and the final endpoint connection handler,
 * determining whether the message should be passed on to the handler.
 */
class QWSENGINE_EXPORT Middleware : public QObject {
    Q_OBJECT

 public:
    /**
     * @brief Base constructor for middleware
     */
    explicit Middleware(QObject *parent = nullptr) : QObject(parent) {}

    // virtual ~Middleware();

    /**
     * @brief Name of the middleware for logging purposes
     */
    virtual QString name() const = 0;

    /**
     * @brief Determine if request processing should continue
     *
     * This method is invoked when a new request comes in. If true is
     * returned, processing continues. Otherwise, it is assumed that an
     * appropriate error was written to the socket.
     */
    virtual bool process(QSharedPointer<Connection> connection, const QString &msgName, const QVariant &message) = 0;
};

}  // namespace QWsEngine
