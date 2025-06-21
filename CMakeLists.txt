cmake_minimum_required(VERSION 3.19)
project(ChatClient LANGUAGES CXX)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql Network)

qt_standard_project_setup()

include_directories(
    src
)

qt_add_resources(ChatClient_RESOURCES resources.qrc)

qt_add_executable(ChatClient
        WIN32 MACOSX_BUNDLE
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        resources.qrc

        # MainPage 页面
        Pages/MainPage/chatmain.cpp
        Pages/MainPage/chatmain.h
        Pages/MainPage/chatmain.ui

        # 其他页面
        Pages/RegisterPage/register.cpp
        Pages/RegisterPage/register.h
        Pages/RegisterPage/register.ui
        Pages/LoginPage/login.cpp
        Pages/LoginPage/login.h
        Pages/LoginPage/login.ui
        Pages/ForgetPwd/forgetpwd.cpp
        Pages/ForgetPwd/forgetpwd.h
        Pages/ForgetPwd/forgetpwd.ui

        # 源文件
        src/Client.cpp
        src/Common.cpp
        src/Buffer.cpp
        src/LoginReq.cpp
        src/Request.cpp
        src/Response.cpp
        src/RegisterReq.cpp

        # 主程序
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui

        # 资源文件
        ${ChatClient_RESOURCES}
        src/HeartBeatReq.cpp
        src/HeartBeatReq.h
        src/ChatDelegate.h
        src/ChatDelegate.cpp
        src/SendMsgReq.cpp
        src/SendMsgReq.h
        src/LoadUserInfo.cpp
        src/LoadUserInfo.h
)

target_link_libraries(ChatClient
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Sql
        Qt6::Network
)

include(GNUInstallDirs)

install(TARGETS ChatClient
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ChatClient
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
