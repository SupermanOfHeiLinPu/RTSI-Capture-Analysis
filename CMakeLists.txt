cmake_minimum_required (VERSION 3.22.1)

project (rtsi_analysis)

set(CMAKE_CXX_STANDARD 14)

add_executable(
    ${PROJECT_NAME}
    RtsiReciepSetup.cpp
    RtsiRecipe.cpp
    RtsiParserData.cpp
    RtsiParser.cpp
    RtsiCapture.cpp
    TcpMessage.cpp
    TcpCapture.cpp
    main.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ./
)

target_link_libraries(
    ${PROJECT_NAME}
    libpcap.a
)
