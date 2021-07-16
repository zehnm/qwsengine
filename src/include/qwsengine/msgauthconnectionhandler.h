// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/connectionhandler.h>

#include "qwsengine_export.h"

namespace QWsEngine {

class MsgAuthConnectionHandlerPrivate;

/**
 * @brief Connection handler for header based authentication.
 */
class QWSENGINE_EXPORT MsgAuthConnectionHandler : public ConnectionHandler {
    Q_OBJECT

 public:
    /**
     * @brief Creates a new connection authentication handler
     */
    explicit MsgAuthConnectionHandler(QObject *parent = 0);
    explicit MsgAuthConnectionHandler(Handler *handler, QObject *parent = nullptr);
    virtual ~MsgAuthConnectionHandler();

    QString name() const override;

    /**
     * @brief Sets the error behaviour in case of failed header authentication.
     *
     * If set to false this handler can be used in a chained authentication setup.
     */
    virtual void setFailedAuthClosesConnection(bool close);
    virtual bool isFailedAuthClosesConnection() const;

 protected:
    QSharedPointer<Connection> createConnection(QWebSocket *socket, bool authenticated = false) override;

 private:
    MsgAuthConnectionHandlerPrivate *const d;
    friend class MsgAuthConnectionHandlerPrivate;
};

}  // namespace QWsEngine
