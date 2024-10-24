cmake_minimum_required(VERSION 3.16)

project(CA1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OPENSSL_ROOT_DIR "C:/Qt/Tools/OpenSSLv3/Win_x64")
set(OPENSSL_INCLUDE_DIR "C:/Qt/Tools/OpenSSLv3/Win_x64/include")
set(OPENSSL_LIBRARIES "C:/Qt/Tools/OpenSSLv3/Win_x64/lib")
# set(Boost_INCLUDE_DIR "C:/Program Files/Boost/boost_1_82_0")

# find_package(Threads REQUIRED)
find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Multimedia)
# find_package(Boost REQUIRED)

find_package(OpenSSL REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(P2PVoiceCall
    main.cpp
)

qt_add_qml_module(P2PVoiceCall
    URI P2PVoiceCall
    VERSION 1.0
    SOURCES webrtc.cpp webrtc.h
    SOURCES audioinput.cpp audioinput.h audiooutput.cpp audiooutput.h
    SOURCES socket.io-client-cpp/src/sio_client.cpp socket.io-client-cpp/src/sio_client.h socket.io-client-cpp/src/sio_message.h socket.io-client-cpp/src/sio_socket.cpp socket.io-client-cpp/src/sio_socket.h
    SOURCES socket.io-client-cpp/src/internal/sio_client_impl.cpp socket.io-client-cpp/src/internal/sio_client_impl.h socket.io-client-cpp/src/internal/sio_packet.cpp socket.io-client-cpp/src/internal/sio_packet.h
    SOURCES signalmanager.h signalmanager.cpp
    RESOURCES resources.qrc
)
set_target_properties(P2PVoiceCall PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS P2PVoiceCall
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(EXISTS "${CMAKE_SOURCE_DIR}/path")
    include(${CMAKE_SOURCE_DIR}/path)
else()
    message(WARNING "Path file not found. Please create it to define local paths.")
endif()

add_definitions(-D_WEBSOCKETPP_CPP11_STL_)
add_definitions(-D_WEBSOCKETPP_CPP11_FUNCTIONAL_)
# add_definitions(-DSIO_TLS)
add_definitions(-DBOOST_DATE_TIME_NO_LIB)
add_definitions(-DBOOST_REGEX_NO_LIB)
add_definitions(-DASIO_STANDALONE)
add_definitions(-DQT_NO_KEYWORDS)

include_directories(libdatachannel/include)
include_directories(opus/include)
include_directories(socket.io-client-cpp/lib/websocketpp)
include_directories(socket.io-client-cpp/lib/asio/asio/include)
include_directories(socket.io-client-cpp/lib/rapidjson/include)
include_directories(${OPENSSL_INCLUDE_DIR})

# include_directories(${OPENSSL_INCLUDE_DIR})

# target_include_directories(P2PVoiceCall PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(P2PVoiceCall PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(P2PVoiceCall PRIVATE ${PATH}/libdatachannel/Windows/Mingw64/libdatachannel.dll.a)
target_link_libraries(P2PVoiceCall PRIVATE C:/Qt/Tools/OpenSSLv3/Win_x64/bin/libcrypto-3-x64.dll)
target_link_libraries(P2PVoiceCall PRIVATE C:/Qt/Tools/OpenSSLv3/Win_x64/bin/libssl-3-x64.dll)
target_link_libraries(P2PVoiceCall PRIVATE ${PATH}/opus/Windows/Mingw64/libopus.a)
target_link_libraries(P2PVoiceCall PRIVATE ws2_32 ssp)
target_link_libraries(P2PVoiceCall PRIVATE Qt6::Quick Qt6::Multimedia)
# target_link_libraries(P2PVoiceCall PRIVATE Threads::Threads)
# target_link_libraries(P2PVoiceCall PRIVATE OpenSSL::SSL OpenSSL::Crypto)


