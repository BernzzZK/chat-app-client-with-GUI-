cmake_minimum_required(VERSION 3.19)
project(ChatClient LANGUAGES CXX)

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
    src/Client.cpp
    src/Common.cpp
    Pages/RegisterPage/register.cpp
    Pages/RegisterPage/register.h
    Pages/RegisterPage/register.ui
    Pages/LoginPage/login.cpp
    Pages/LoginPage/login.h
    Pages/LoginPage/login.ui
    src/Buffer.cpp
    src/LoginReq.cpp
    src/Request.cpp
    src/Response.cpp
    src/RegisterReq.cpp
    resources.qrc
    ${ChatClient_RESOURCES}
        Pages/ForgetPwd/forgetpwd.cpp
        Pages/ForgetPwd/forgetpwd.h
        Pages/ForgetPwd/forgetpwd.ui
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
