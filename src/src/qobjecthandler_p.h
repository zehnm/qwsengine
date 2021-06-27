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

#pragma once

#include <QMap>
#include <QObject>
#include <QRegExp>

namespace QWsEngine {

class Connection;
class QObjectHandler;

class QObjectHandlerPrivate : public QObject {
    Q_OBJECT

 public:
    explicit QObjectHandlerPrivate(QObjectHandler *handler);

    // In order to invoke the slot, a "pointer" to it needs to be stored in a
    // map that lets us look up information by method name

    class Method {
     public:
        Method() {}
        Method(QObject *receiver, const char *method) : receiver(receiver), oldSlot(true), slot(method) {}
        Method(QObject *receiver, QtPrivate::QSlotObjectBase *slotObj)
            : receiver(receiver), oldSlot(false), slot(slotObj) {}

        QObject *receiver;
        bool     oldSlot;
        union slot {
            slot() {}
            slot(const char *method) : method(method) {}
            slot(QtPrivate::QSlotObjectBase *slotObj) : slotObj(slotObj) {}
            const char *                method;
            QtPrivate::QSlotObjectBase *slotObj;
        } slot;
    };

    void invokeSlot(Connection *socket, const QVariant &message, Method m);

    QMap<QString, Method> map;

 private:
    QObjectHandler *const q;
};

}  // namespace QWsEngine
