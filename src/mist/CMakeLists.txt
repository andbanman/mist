set(include_path "${public_include_path}/${PROJECT_NAME}")

#
# Macros and Functions
#
function(add_object name)
    set(target ${name})
    set(src ${ARGN})
    add_library(${target} OBJECT ${src})
    target_compile_options(${target} PRIVATE "-fPIC")
    target_include_directories(${target} PRIVATE ${include_path})
    if (${BuildPython})
        target_include_directories(${name} PRIVATE ${PYTHON_INCLUDE_DIRS})
        target_compile_definitions(${name} PRIVATE BOOST_PYTHON_EXTENSIONS)
    endif()
endfunction()

macro(add_unit_test name)
    set(target ${name}.test)
    set(src ${ARGN})
    add_executable(${target} ${src})
    target_include_directories(${target} PRIVATE ${include_path})
    target_link_libraries(${target}
        Boost::filesystem
        Boost::serialization
        Boost::unit_test_framework
    )
    target_compile_definitions(${target} PRIVATE
        BOOST_TEST_MAIN
        BOOST_TEST_MODULE=${target}
    )
    if (${BuildPython})
        target_include_directories(${target} PRIVATE
            ${PYTHON_INCLUDE_DIRS}
            ${NUMPY_INCLUDE_DIRS}
            ${Boost_INCLUDE_DIRS}
            )
        target_link_libraries(${target} ${PYTHON_LIBRARIES} ${BOOST_PYTHON_LIBRARIES} ${BOOST_NUMPY_LIBRARIES})
    endif()
    if (NOT ${BuildStatic})
        target_compile_definitions(${target} PRIVATE BOOST_TEST_DYN_LINK)
    endif()
    add_test(NAME ${target} COMMAND ${target})
endmacro()

macro(add_namespace_object name)
    add_object("${namespace}${name}" ${name}.cpp ${include_path}/${namespace}/${name}.hpp ${ARGN})
    list(APPEND ${namespace}_objects "$<TARGET_OBJECTS:${namespace}${name}>")
endmacro()

macro(add_namespace_test name)
    add_unit_test("${namespace}${name}" ${name}.test.cpp $<TARGET_OBJECTS:${namespace}${name}> ${ARGN})
endmacro()

#
# Build
#
# Core Objects. These are allowed to be linked to
add_object(Permutation Permutation.cpp ${include_path}/Permutation.hpp)
add_object(Variable Variable.cpp ${include_path}/Variable.hpp)
add_object(Mist Mist.cpp ${include_path}/Mist.hpp ${include_path}/mist.hpp)

# Namespaces
add_subdirectory(algorithm)
add_subdirectory(cache)
add_subdirectory(io)
add_subdirectory(it)

# TODO use generator expressions instead
if (${BuildStatic})
    set(library_type "STATIC")
    set(library_dest "ARCHIVE")
else()
    set(library_type "SHARED")
    set(library_dest "LIBRARY")
endif()

set(library_objects
    $<TARGET_OBJECTS:Mist>
    $<TARGET_OBJECTS:Permutation>
    $<TARGET_OBJECTS:Variable>
    ${algorithm_objects}
    ${cache_objects}
    ${io_objects}
    ${it_objects}
)

# Final library target
if (NOT ${BuildPython})
    add_library(${PROJECT_NAME} ${library_type} ${library_objects})
    target_include_directories(${PROJECT_NAME} PUBLIC ${include_path})
    install(TARGETS ${PROJECT_NAME} ${library_dest} DESTINATION lib/${PROJECT_NAME})
    if (${InstallExtern})
        set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH $ORIGIN/extern)
        resolve_symlinks(install_extern_libraries ${extern_libraries})
        install(FILES ${install_extern_libraries} DESTINATION lib/${PROJECT_NAME}/extern)
    endif()

else()
    # The built library will contain Python extensions
    set(python_library lib${PROJECT_NAME})
    PYTHON_ADD_MODULE(${python_library} "Pymist.cpp" ${library_objects})
    target_compile_definitions(${python_library} PRIVATE BOOST_PYTHON_EXTENSIONS)
    target_include_directories(${python_library} PRIVATE
        ${include_path}
        ${PYTHON_INCLUDE_DIRS}
        ${NUMPY_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
    )
    target_link_libraries(${python_library} ${BOOST_PYTHON_LIBRARIES} ${BOOST_NUMPY_LIBRARIES})
    install(TARGETS ${python_library} LIBRARY DESTINATION ${INSTALL_LIB_PYTHON_PATH})
    if (${InstallExtern})
        set_target_properties(${python_library} PROPERTIES INSTALL_RPATH $ORIGIN/extern)
        resolve_symlinks(install_extern_libraries ${extern_libraries})
        install(FILES ${install_extern_libraries} DESTINATION ${INSTALL_LIB_PYTHON_PATH}/extern)
    endif()
endif()

#
# Unit Tests
#
if(${BuildTest})
    add_unit_test(Permutation
        Permutation.test.cpp
        $<TARGET_OBJECTS:Permutation>
        )

    add_unit_test(Variable
        Variable.test.cpp
        $<TARGET_OBJECTS:Variable>
        )

    add_unit_test(VariablePermutation
        VariablePermutation.test.cpp
        $<TARGET_OBJECTS:Permutation>
        $<TARGET_OBJECTS:Variable>
        )
endif()
