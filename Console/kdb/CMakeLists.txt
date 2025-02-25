cmake_minimum_required (VERSION 3.8)

project (kdb VERSION 0.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD           17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS        OFF)

if(NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE "Release" "Debug")
endif()

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

set(SOURCE
	${CMAKE_SOURCE_DIR}/src/Framework.hpp
	${CMAKE_SOURCE_DIR}/src/main.cpp
	${CMAKE_SOURCE_DIR}/src/menu.cpp
	${CMAKE_SOURCE_DIR}/src/Menu.hpp
	${CMAKE_SOURCE_DIR}/src/movie.cpp
	${CMAKE_SOURCE_DIR}/src/Movie.hpp
	${CMAKE_SOURCE_DIR}/src/MovieException.hpp
	${CMAKE_SOURCE_DIR}/src/movie_db.cpp
	${CMAKE_SOURCE_DIR}/src/MovieDatabase.hpp
	${CMAKE_SOURCE_DIR}/src/schema.cpp
	${CMAKE_SOURCE_DIR}/src/Schema.hpp
	${CMAKE_SOURCE_DIR}/src/criteria.cpp
	${CMAKE_SOURCE_DIR}/src/Criteria.hpp
	${CMAKE_SOURCE_DIR}/src/Utils.hpp
	${CMAKE_SOURCE_DIR}/src/rlutil.h
)

add_executable(kdb ${SOURCE})
if (MSVC)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT kdb)
	set_target_properties(kdb PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build")
	set_target_properties(kdb PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif(MSVC)

file(COPY ${CMAKE_SOURCE_DIR}/import/movies.txt DESTINATION ${CMAKE_BINARY_DIR}/import)
file(COPY ${CMAKE_SOURCE_DIR}/import/movies.csv DESTINATION ${CMAKE_BINARY_DIR}/import)
file(COPY ${CMAKE_SOURCE_DIR}/ico/kdb.ico DESTINATION ${CMAKE_BINARY_DIR}/ico)
file(COPY ${CMAKE_SOURCE_DIR}/ico/codeit.ico DESTINATION ${CMAKE_BINARY_DIR}/ico)
file(COPY ${CMAKE_SOURCE_DIR}/codeit/codeit.exe DESTINATION ${CMAKE_BINARY_DIR}/codeit)


