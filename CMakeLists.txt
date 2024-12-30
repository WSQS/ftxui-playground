cmake_minimum_required(VERSION 3.22)

project(ftxui-playground
        LANGUAGES CXX
        VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/WSQS/FTXUI.git
        GIT_PROGRESS TRUE
        GIT_SHALLOW TRUE
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(ftxui)
# ------------------------------------------------------------------------------
# --- Fetch Boost --------------------------------------------------------------
find_package(Boost 1.83 QUIET COMPONENTS system filesystem)

message(STATUS "Boost found: ${Boost_FOUND}")
message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
else ()
    FetchContent_Declare(
            Boost
            GIT_REPOSITORY https://github.com/boostorg/boost.git
            GIT_TAG boost-1.87.0
    )
    FetchContent_MakeAvailable(Boost)
endif()
# ------------------------------------------------------------------------------
file(GLOB SOURCES "src/*.cpp")
add_executable(ftxui-playground ${SOURCES} "./main.cpp")
target_include_directories(ftxui-playground PRIVATE include)
target_link_libraries(ftxui-playground
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component # Not needed for this example.
        PRIVATE Boost::process
)

set_target_properties(ftxui-playground PROPERTIES CXX_STANDARD 11)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(ftxui-playground PRIVATE /W4 /WX-)
else ()
    target_compile_options(ftxui-playground PRIVATE -Wall -Wextra -pedantic -Wno-error)
endif ()

install(TARGETS ftxui-playground RUNTIME DESTINATION "bin")
