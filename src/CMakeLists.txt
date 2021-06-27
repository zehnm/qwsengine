configure_file(qwsengine_export.h.in "${CMAKE_CURRENT_BINARY_DIR}/qwsengine_export.h")

set(HEADERS
    include/qwsengine/authmiddleware.h
    include/qwsengine/connection.h
    include/qwsengine/connectionhandler.h
    include/qwsengine/connectionmiddleware.h
    include/qwsengine/handler.h
    include/qwsengine/headerauthconnectionhandler.h
    include/qwsengine/middleware.h
    include/qwsengine/msgauthconnectionhandler.h
    include/qwsengine/msgauthmiddleware.h
    include/qwsengine/qobjecthandler.h
    include/qwsengine/server.h
    include/qwsengine/tokenauthenticator.h
    "${CMAKE_CURRENT_BINARY_DIR}/qwsengine_export.h"
)

set(SRC
    src/authmiddleware.cpp
    src/connection.cpp
    src/connectionhandler.cpp
    src/handler.cpp
    src/headerauthconnectionhandler.cpp
    src/msgauthconnectionhandler.cpp
    src/msgauthmiddleware.cpp
    src//qobjecthandler.cpp
    src/server.cpp
    src/wslogging.cpp
)

if(WIN32)
    configure_file(resource.rc.in "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
    set(SRC ${SRC} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
endif()

add_library(qwsengine ${HEADERS} ${SRC})

set_target_properties(qwsengine PROPERTIES
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
    DEFINE_SYMBOL         QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL         QWSENGINE_LIBRARY
    PUBLIC_HEADER         "${HEADERS}"
    VERSION               ${PROJECT_VERSION}
    SOVERSION             ${PROJECT_VERSION_MAJOR}
)

target_include_directories(qwsengine PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
)

target_link_libraries(qwsengine Qt5::WebSockets)

install(TARGETS qwsengine EXPORT qwsengine-export
    RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INCLUDE_INSTALL_DIR}/qwsengine"
)

install(EXPORT qwsengine-export
    FILE        qwsengineConfig.cmake
    DESTINATION "${LIB_INSTALL_DIR}/cmake/qwsengine"
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/qwsengineConfigVersion.cmake"
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qwsengineConfigVersion.cmake"
    DESTINATION "${LIB_INSTALL_DIR}/cmake/qwsengine"
)

configure_file(qwsengine.pc.in "${CMAKE_CURRENT_BINARY_DIR}/qwsengine.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qwsengine.pc"
    DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
)
