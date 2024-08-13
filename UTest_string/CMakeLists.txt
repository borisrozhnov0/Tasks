CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )

project(utest_string 
    VERSION 1.0.0
    LANGUAGES CXX)

find_package(Boost 1.36.0)

if(NOT Boost_FOUND)
    set(BOOST_ROOT "C:\\Program Files\\boost\\boost_1_85_0")
    find_package(Boost 1.36.0)
endif()

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(BEFORE 
    ${PROJECT_SOURCE_DIR}/include 
    ${PROJECT_SOURCE_DIR}/src)

add_executable(utest_string utest_string_h.cpp)
