cmake_minimum_required(VERSION 2.6)
project(g35d)

set(CMAKE_INSTALL_PREFIX /usr/local/)

# When building libg35 and g35d side by side
include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/libs)

add_executable(g35d g35d.c g35config.c)
target_link_libraries(g35d g35 pthread confuse)

install(TARGETS g35d
    DESTINATION "bin"
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
install(FILES g35d.rc
    DESTINATION "/etc"
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
