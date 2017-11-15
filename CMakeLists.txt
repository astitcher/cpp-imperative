#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#
cmake_minimum_required (VERSION 2.8.7)

enable_language(CXX)

find_package(ProtonCpp REQUIRED)

include_directories(${ProtonCpp_INCLUDE_DIRS})
link_libraries(${ProtonCpp_LIBRARIES})
add_definitions(${ProtonCpp_DEFINITIONS})

set (BUILD_CPP_03 OFF CACHE BOOL "Compile as C++03 even when C++11 is available")
# This effectively checks for cmake version 3.1 or later
if (DEFINED CMAKE_CXX_COMPILE_FEATURES)
  if (BUILD_CPP_03 OR MSVC)
    set(STD 98)
  else ()
    set(STD 11)
    set(HAS_CPP11 ON)
  endif ()
  set(CMAKE_CXX_STANDARD ${STD})
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# Single-threaded examples that work on C++03
foreach(example
    client
    server)
  add_executable(${example} ${example}.cpp)
endforeach()

