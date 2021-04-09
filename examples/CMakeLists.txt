set(include_path "${public_include_path}")

# Always use static boost libraries for binaries to simplify library loading
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS
    program_options
)
find_package(Threads)

macro(add_example name)
    set(target ${name})
    set(src ${ARGN})
    add_executable(${target} ${src})
    target_include_directories(${target} PRIVATE ${include_path})
    target_link_libraries(${target} ${PROJECT_NAME})
    install(TARGETS ${target} RUNTIME DESTINATION bin/)
endmacro()

add_example(mistcli mist.cpp)

# add 3rd-party dependency
add_subdirectory(../extras/yaml-cpp ${CMAKE_CURRENT_BINARY_DIR}/ext/yaml-cpp)
add_dependencies(mistcli yaml-cpp)
target_link_libraries(mistcli Boost::program_options ${CMAKE_BINARY_DIR}/lib/libyaml-cpp.a)
target_include_directories(mistcli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../extras/yaml-cpp/include)

if (${BuildStatic})
    # To static link pthreads you must deal with weak symbols
    # https://stackoverflow.com/a/45271521
    # TODO make more dynamic
    target_link_libraries(mistcli "-lrt -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")
else()
    target_link_libraries(mistcli Threads::Threads)
endif()
