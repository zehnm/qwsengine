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

#include <qwsengine/handler.h>

#include "qwsengine_export.h"

namespace QWsEngine {

class Connection;

class QObjectHandlerPrivate;

/**
 * @brief %Handler for invoking slots
 *
 * This handler enables incoming requests to be processed by slots in a
 * QObject-derived class or functor. Methods are registered by providing a
 * name and slot to invoke. The slot must take a pointer to the
 * [Socket](@ref QWsEngine::Socket) for the request as an argument and
 * must also close the socket when finished with it.
 *
 * To use this class, simply create an instance and call the appropriate
 * registerMessage() overload. For example:
 *
 * @code
 * class Object : public QObject
 * {
 *     Q_OBJECT
 * public slots:
 *     void something(QWsEngine::Socket *socket);
 * };
 *
 * QWsEngine::QObjectHandler handler;
 * Object object;
 * // Old connection syntax
 * handler.registerMessage("something", &object, SLOT(something(QWsEngine::Socket*)));
 * // New connection syntax
 * handler.registerMessage("something", &object, &Object::something);
 * @endcode
 *
 * It is also possible to use this class with a functor, eliminating the need
 * to create a class and slot:
 *
 * @code
 * QWsEngine::QObjectHandler handler;
 * handler.registerMessage("something", [](QWsEngine::Socket *socket) {
 *     // do something
 *     socket->close();
 * });
 * @endcode
 */
class QWSENGINE_EXPORT QObjectHandler : public Handler {
    Q_OBJECT

 public:
    /**
     * @brief Create a new QObject handler
     */
    explicit QObjectHandler(QObject *parent = 0);

    /**
     * @brief Register a method
     *
     * This overload uses the traditional connection syntax with macros.
     */
    void registerMessage(const QString &name, QObject *receiver, const char *method);

#ifdef DOXYGEN
    /**
     * @brief Register a method
     *
     * This overload uses the new connection syntax with member pointers.
     */
    void registerMessage(const QString &name, QObject *receiver, PointerToMemberFunction method);

    /**
     * @brief Register a method
     *
     * This overload uses the new functor syntax (without context).
     */
    void registerMessage(const QString &name, Functor functor);

    /**
     * @brief Register a method
     *
     * This overload uses the new functor syntax (with context).
     */
    void registerMessage(const QString &name, QObject *receiver, Functor functor);
#else
    template <typename Func1>
    inline void registerMessage(const QString &name, typename QtPrivate::FunctionPointer<Func1>::Object *receiver,
                                Func1 slot) {
        typedef QtPrivate::FunctionPointer<Func1> SlotType;

        // Ensure the slot doesn't have too many arguments
        Q_STATIC_ASSERT_X(static_cast<int>(SlotType::ArgumentCount) == 2, "The slot must have exactly two argument.");

        // Ensure the argument is of the correct type
        Q_STATIC_ASSERT_X(
            (QtPrivate::AreArgumentsCompatible<
                Connection *, typename QtPrivate::List_Select<typename SlotType::Arguments, 0>::Value>::value),
            "The slot parameters do not match");

        // Invoke the implementation
        registerMessageImpl(name, receiver,
                            new QtPrivate::QSlotObject<Func1, typename SlotType::Arguments, void>(slot));
    }

    template <typename Func1>
    inline typename QtPrivate::QEnableIf<!QtPrivate::AreArgumentsCompatible<Func1, QObject *>::value, void>::Type
    registerMessage(const QString &name, Func1 slot) {
        registerMessage(name, Q_NULLPTR, slot);
    }

    template <typename Func1>
    inline typename QtPrivate::QEnableIf<!QtPrivate::FunctionPointer<Func1>::IsPointerToMemberFunction &&
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
                                             !std::is_same<const char *, Func1>::value,
#else
                                             !QtPrivate::is_same<const char *, Func1>::value,
#endif
                                         void>::Type
    registerMessage(const QString &name, QObject *context, Func1 slot) {
        // There is an easier way to do this but then the header wouldn't
        // compile on non-C++11 compilers
        return registerMessage_functor(name, context, slot, &Func1::operator());
    }
#endif

 protected:
    /**
     * @brief Reimplementation of [Handler::process()](QWsEngine::Handler::process)
     */
    void process(Connection *connection, const QString &msgName, const QVariant &message) override;

 private:
    template <typename Func1, typename Func1Operator>
    inline void registerMessage_functor(const QString &name, QObject *context, Func1 slot, Func1Operator) {
        typedef QtPrivate::FunctionPointer<Func1Operator> SlotType;

        // Ensure the slot doesn't have too many arguments
        Q_STATIC_ASSERT_X(static_cast<int>(SlotType::ArgumentCount) == 2, "The slot must have exactly two arguments.");

        // Ensure the argument is of the correct type
        Q_STATIC_ASSERT_X(
            (QtPrivate::AreArgumentsCompatible<
                Connection *, typename QtPrivate::List_Select<typename SlotType::Arguments, 0>::Value>::value),
            "The first slot parameter does not match");
        Q_STATIC_ASSERT_X(
            (QtPrivate::AreArgumentsCompatible<
                const QVariant &, typename QtPrivate::List_Select<typename SlotType::Arguments, 1>::Value>::value),
            "The second slot parameter does not match");

        registerMessageImpl(name, context,
                            new QtPrivate::QFunctorSlotObject<Func1, 2, typename SlotType::Arguments, void>(slot));
    }

    void registerMessageImpl(const QString &name, QObject *receiver, QtPrivate::QSlotObjectBase *slotObj);

    QObjectHandlerPrivate *const d;
    friend class QObjectHandlerPrivate;
};

}  // namespace QWsEngine
