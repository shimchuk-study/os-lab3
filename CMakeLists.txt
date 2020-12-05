cmake_minimum_required(VERSION 3.5)

project(os_lab3 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package(Boost 1.67.0 COMPONENTS program_options) 

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS}) 
  add_executable(os_lab3 
                  src/main.cpp
                  src/os_simulation.cpp
                  src/os_simulation.hpp
                )
  target_link_libraries(os_lab3 Boost::program_options)
else()
  message(FATAL_ERROR "boost not found")
endif()
