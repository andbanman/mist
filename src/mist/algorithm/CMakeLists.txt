set(namespace "algorithm")
set(algorithm_objects "")

add_namespace_object(TupleSpace)
add_namespace_object(Worker)

set(algorithm_objects ${algorithm_objects} PARENT_SCOPE)

if(${BuildTest})
    add_namespace_test(TupleSpace $<TARGET_OBJECTS:Variable>)
endif()
