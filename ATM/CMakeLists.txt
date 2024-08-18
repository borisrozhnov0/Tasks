CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )

PROJECT(
    ATM 
    VERSION 0.0.1 
    LANGUAGES CXX)

include_directories(
    D:/Tools/pgsql/include
)
add_executable(main main.cxx)
target_link_libraries(main D:/Tools/pgsql/lib/libpq.lib)