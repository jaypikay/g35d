cmake_minimum_required(VERSION 2.8)
project(g35d)

set(CMAKE_INSTALL_PREFIX /usr/local/)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

# When building libg35 and g35d side by side
include_directories(${SOURCE_DIR}/include)
link_directories(${SOURCE_DIR}/libs)

set(G35D_MAN_SRC
    ${SOURCE_DIR}/manpages/g35d.1
    ${SOURCE_DIR}/manpages/g35d.rc.5)

add_executable(g35d g35d.c g35config.c)
target_link_libraries(g35d g35 pthread confuse)

foreach(manpage ${G35D_MAN_SRC})
    get_filename_component(basename ${manpage} NAME)
    string(REGEX REPLACE "^.*\\.([0-9])$" "\\1" section ${basename})

    set(MANPAGE_DIR ${BUILD_DIR}/man/man${section})
    file(MAKE_DIRECTORY ${MANPAGE_DIR})

    set(gzfile ${MANPAGE_DIR}/${basename}.gz)

    add_custom_command(
        COMMAND groff -man -Tascii ${manpage} | gzip -c >${gzfile}
        WORKING_DIRECTORY ${MANPAGE_DIR}
        OUTPUT  ${gzfile}
        DEPENDS ${manpage})

    set(MANPAGES ${MANPAGES} ${gzfile})
endforeach()
add_custom_target(man ALL DEPENDS ${MANPAGES})

install(TARGETS g35d
    DESTINATION "bin"
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
install(FILES g35d.rc
    DESTINATION "/etc"
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
install(DIRECTORY ${BUILD_DIR}/man/man1 DESTINATION "man"
    FILES_MATCHING PATTERN "*.gz")
install(DIRECTORY ${BUILD_DIR}/man/man5 DESTINATION "man"
    FILES_MATCHING PATTERN "*.gz")
