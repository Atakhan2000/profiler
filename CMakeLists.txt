cmake_minimum_required(VERSION 3.1)
project(profiler)

add_library(profiler INTERFACE)
target_compile_features(profiler INTERFACE cxx_thread_local cxx_constexpr cxx_auto_type)
target_compile_options(profiler INTERFACE -Wall -Wextra -Wpedantic -Werror)
target_include_directories(profiler INTERFACE ${PROJECT_SOURCE_DIR})

find_package(Boost REQUIRED)
target_compile_definitions(profiler
  INTERFACE
  BOOST_CHRONO_HEADER_ONLY
  BOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
  BOOST_ERROR_CODE_HEADER_ONLY
  )
target_include_directories(profiler INTERFACE ${Boost_INCLUDE_DIRS})

option(FP_ENABLE "Enable profiler" ON)
target_compile_definitions(profiler INTERFACE $<$<BOOL:${FP_ENABLE}>:FP_ENABLE>)

############
#   Test   #
############
find_package(Threads REQUIRED)

add_library(snippets SHARED test/snippets.cpp)
set_target_properties(snippets PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
target_include_directories(snippets INTERFACE test)
target_compile_options(snippets PUBLIC -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(snippets INTERFACE profiler)

add_executable(main test/main.cpp)
target_link_libraries(main snippets Threads::Threads)
