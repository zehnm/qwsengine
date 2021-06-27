# QWsEngine

Simple set of classes for developing WebSocket server applications in Qt.  
Tailored to JSON message payload in WebSocket text messages.

Inspired by [QHTTPEngine](https://github.com/nitroshare/qhttpengine).

**Goals:**

Simplify larger WebSocket server projects:

- less boilerplate code
- structured message handling instead of the usual do-it-all uber-method.
- focus on writing business logic

**Vision:**

Use this library in combination with an [AsyncAPI](https://www.asyncapi.com/) code generator to create server stub code for an API-first approach.
Similar to the [cpp-qt-qhttpengine-server OpenAPI generator](https://openapi-generator.tech/docs/generators/cpp-qt-qhttpengine-server/) for REST APIs.

## Current State

Still under development, used as proof-of-concept implementation in [YIO remote v2](https://www.yio-remote.com/) rewrite.  
There will be further development and possible refactorings before the initial v0.1.0 release!

Open issues and TODOs:

- [ ] Provide examples
- [ ] Documentation
- [ ] Test suite
- [ ] Investigate smart pointers instead of passing raw pointers around
