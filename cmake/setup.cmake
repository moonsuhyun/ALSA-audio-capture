
link_libraries      (/usr/lib/x86_64-linux-gnu/libasound.so)

message             ("Downloading and extracting ALSA headers...")

if    (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/alsa-lib)

    file                (DOWNLOAD https://www.alsa-project.org/files/pub/lib/alsa-lib-1.2.9.tar.bz2 ${CMAKE_CURRENT_SOURCE_DIR}/lib/alsa-lib-1.2.9.tar.bz2)

    execute_process     (COMMAND  tar xvf alsa-lib-1.2.9.tar.bz2 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

    file                (RENAME   ${CMAKE_CURRENT_SOURCE_DIR}/lib/alsa-lib-1.2.9 ${CMAKE_CURRENT_SOURCE_DIR}/lib/alsa-lib)

    file                (REMOVE   ${CMAKE_CURRENT_SOURCE_DIR}/lib/alsa-lib-1.2.9.tar.bz2)

endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/lib/alsa-lib/include)
