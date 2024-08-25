CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )

PROJECT(
    ATM 
    VERSION 0.1.0 
    LANGUAGES CXX)

set(LIBPQ_INLCUDE_PATH D:/Tools/pgsql/include)

include_directories(    BEFORE
    ${PROJECT_SOURCE_DIR}/include
    D:/Tools/pgsql/include
)

add_library(atm SHARED
    ${PROJECT_SOURCE_DIR}/lib/libpq.lib
    ${LIBPQ_INLCUDE_PATH}/libpq-fe.h
    include/atm.h
    src/ATM_Interface.cpp 
    src/ATM_MSG.cpp 
    src/ATM.cpp )

add_executable(main main.cxx)
target_link_libraries(atm ${PROJECT_SOURCE_DIR}/lib/libpq.lib)

target_link_libraries(main 
    atm
	${PROJECT_SOURCE_DIR}/lib/libpq.lib)

install(FILES lib/libintl-9.dll lib/libpq.dll
    DESTINATION ${PROJECT_SOURCE_DIR}/bin)

install(TARGETS main atm
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin
    LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/bin)
