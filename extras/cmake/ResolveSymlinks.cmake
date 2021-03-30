function(resolve_symlinks destination)
    set(newfiles "")
    foreach(file IN ITEMS ${ARGN})
        set(filepath "${file}")
        if (UNIX)
            execute_process(COMMAND readlink -f ${file} OUTPUT_VARIABLE filepath)
            string(STRIP ${filepath} filepath)
        endif(UNIX)
        list(APPEND newfiles "${filepath}")
        #TODO windows
        # https://stackoverflow.com/questions/13298414/native-alternative-for-readlink-on-windows
    endforeach()
    set(${destination} ${newfiles} PARENT_SCOPE)
endfunction()
