ADD_SUBDIRECTORY(deps/nail)

ADD_EXECUTABLE(ncm_main)

SET_TARGET_PROPERTIES(ncm_main PROPERTIES
    OUTPUT_NAME ncm_main
    VERSION 2
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    FOLDER "ncm_main")

SET(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

TARGET_COMPILE_DEFINITIONS(ncm_main PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>)

TARGET_INCLUDE_DIRECTORIES(ncm_main PRIVATE
    "${SRC_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/nail/include"
    "${PROJECT_BINARY_DIR}")

TARGET_INCLUDE_DIRECTORIES(ncm_main SYSTEM PRIVATE
    $<$<PLATFORM_ID:Linux,Darwin>:/usr/include>
    $<$<PLATFORM_ID:Linux,Darwin>:/usr/local/include>)

TARGET_SOURCES(ncm_main PRIVATE
    ${SRC_DIR}/args.cpp
    ${SRC_DIR}/error.cpp
    ${SRC_DIR}/debug.cpp
    ${SRC_DIR}/parse.cpp
    ${SRC_DIR}/config.cpp
    ${SRC_DIR}/vertex.cpp
    ${SRC_DIR}/domain.cpp
    ${SRC_DIR}/mesh.cpp
    ${SRC_DIR}/main.cpp)

TARGET_LINK_DIRECTORIES(ncm_main PRIVATE
    $<$<PLATFORM_ID:Linux,Darwin>:/usr/lib>
    $<$<PLATFORM_ID:Linux,Darwin>:/usr/local/lib>)

TARGET_LINK_LIBRARIES(ncm_main PRIVATE
    nail
    Threads::Threads
    ${CMAKE_DL_LIBS})

