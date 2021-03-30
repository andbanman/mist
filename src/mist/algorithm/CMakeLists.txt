set(namespace "algorithm")
set(algorithm_objects "")

add_namespace_object(BatchQueue)
add_namespace_object(BatchTupleConsumer)
add_namespace_object(BatchTupleProducer)
add_namespace_object(CompletionQueue)
add_namespace_object(CompletionTupleConsumer)
add_namespace_object(CompletionTupleProducer)
add_namespace_object(Coordinator)

set(algorithm_objects ${algorithm_objects} PARENT_SCOPE)

if(${BuildTest})
    # no tests yet...
endif()
