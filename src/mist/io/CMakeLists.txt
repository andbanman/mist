set(namespace "io")
set(io_objects "")

add_namespace_object(DataMatrix)
add_namespace_object(FileOutputStream)
add_namespace_object(FlatOutputStream)
add_namespace_object(MapOutputStream)

set(io_objects ${io_objects} PARENT_SCOPE)

if(${BuildTest})
    add_namespace_test(DataMatrix $<TARGET_OBJECTS:Variable>)
endif()
