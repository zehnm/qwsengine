/*
 * Copyright (c) 2017 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <QGenericArgument>
#include <QMetaMethod>

#include <qwsengine/qobjecthandler.h>
#include <qwsengine/connection.h>

#include "qobjecthandler_p.h"

namespace QWsEngine {

QObjectHandlerPrivate::QObjectHandlerPrivate(QObjectHandler *handler)
    : QObject(handler),
      q(handler) {}

QObjectHandler::QObjectHandler(QObject *parent)
    : Handler(parent),
      d(new QObjectHandlerPrivate(this)) {}

void QObjectHandlerPrivate::invokeSlot(Connection *connection, const QVariant &message, Method m) {
    // Invoke the slot
    if (m.oldSlot) {
        // Obtain the slot index
        int index = m.receiver->metaObject()->indexOfSlot(m.slot.method + 1);
        if (index == -1) {
            connection->sendErrorResponse(500);
            return;
        }

        QMetaMethod method = m.receiver->metaObject()->method(index);

        // Ensure the parameter is correct
        QList<QByteArray> params = method.parameterTypes();
        if (params.count() != 2 || params.at(0) != "WsEngine::Connection*") {
            connection->sendErrorResponse(500);
            return;
        }

        // Invoke the method
        // TODO(zehnm) old slot code not yet tested !!!
        if (!m.receiver->metaObject()->method(index).invoke(
                    m.receiver, Q_ARG(Connection*, connection), Q_ARG(QVariant, message))) {
            connection->sendErrorResponse(500);
            return;
        }
    } else {
        void *args[] = {
            Q_NULLPTR,
            &connection,
            (void *)&message // NOLINT
        };
        m.slot.slotObj->call(m.receiver, args);
    }
}

void QObjectHandler::process(Connection *connection, const QString &msgName, const QVariant &message) {
    // Ensure the method has been registered
    if (!d->map.contains(msgName)) {
        connection->sendErrorResponse(404);
        return;
    }

    QObjectHandlerPrivate::Method m = d->map.value(msgName);

    d->invokeSlot(connection, message, m);
}

void QObjectHandler::registerMessage(const QString &name, QObject *receiver, const char *method) {
    d->map.insert(name, QObjectHandlerPrivate::Method(receiver, method));
}

void QObjectHandler::registerMessageImpl(const QString &name, QObject *receiver, QtPrivate::QSlotObjectBase *slotObj) {
    d->map.insert(name, QObjectHandlerPrivate::Method(receiver, slotObj));
}

}  // namespace QWsEngine
