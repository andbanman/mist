set(namespace "cache")
set(cache_objects "")

add_namespace_object(Flat)
add_namespace_object(Flat1D)
#add_namespace_object(Map)
#add_namespace_object(MRU)
#add_namespace_object(SmallFiles)

set(cache_objects ${cache_objects} PARENT_SCOPE)

#if(${BuildTest})
#    add_namespace_test(MRU)
#    add_namespace_test(SmallFiles)
#endif()
