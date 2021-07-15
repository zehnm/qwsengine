// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2021 Markus Zehnder

#pragma once

#include <qwsengine/connectionhandler.h>
#include <qwsengine/tokenauthenticator.h>

#include "qwsengine_export.h"

namespace QWsEngine {

class QWSENGINE_EXPORT HeaderAuthConnectionHandlerPrivate;

/**
 * @brief Connection handler for header based authentication.
 */
class HeaderAuthConnectionHandler : public ConnectionHandler {
    Q_OBJECT

 public:
    /**
     * @brief Creates a new connection authentication handler
     */
    explicit HeaderAuthConnectionHandler(const QString &tokenName = "X-Auth-Token", QObject *parent = nullptr);
    explicit HeaderAuthConnectionHandler(Handler *handler, const QString &tokenName = "X-Auth-Token",
                                         QObject *parent = nullptr);
    virtual ~HeaderAuthConnectionHandler();

    QString name() const override;

    void    setTokenHeaderName(const QString &headerName);
    QString tokenHeaderName() const;

    void                setTokenAuthenticator(TokenAuthenticator *authenticator);
    TokenAuthenticator *tokenAuthenticator();

    // TODO(zehnm) use enum for error behaviour. Not all combinations make sense!
    /**
     * @brief Sets the error behaviour in case of failed header authentication.
     *
     * If set to false this handler can be used in a chained authentication setup.
     * Otherwise the socket is closed and the process method returns null.
     * Defaults to true.
     */
    void setFailedAuthClosesSocket(bool close);
    bool isFailedAuthClosesSocket() const;

    /**
     * @brief Sets the error behaviour in case of failed header authentication.
     *
     * If set to true a failed authentication still creates a new connection. The
     * message handler is then responsible to deal with authentication.
     * Otherwise the socket is closed and the process method returns null.
     * Defaults to false.
     */
    void setFailedAuthCreatesConnection(bool create);
    bool isFailedAuthCreatesConnection() const;

 protected:
    /**
     * @brief Reimplementation of [ConnectionHandler::process()](QWsEngine::ConnectionHandler::process)
     */
    QSharedPointer<Connection> process(QWebSocket *socket, const QString &path) override;

 private:
    HeaderAuthConnectionHandlerPrivate *const d;
    friend class HeaderAuthConnectionHandlerPrivate;
};

}  // namespace QWsEngine
