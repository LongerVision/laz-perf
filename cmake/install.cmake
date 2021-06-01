include(CMakePackageConfigHelpers)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/lazperf-config-version.cmake
    VERSION ${LAZPERF_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(../cmake/lazperf-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/lazperf-config.cmake
    COPYONLY
)

install(
    FILES
        lazperf/lazperf.hpp
        lazperf/filestream.hpp
        lazperf/vlr.hpp
        lazperf/io.hpp
    DESTINATION
        include/lazperf
)
install(
    FILES
        lazperf/lazperf_user_base.hpp
    DESTINATION
        include/lazperf
    RENAME
        lazperf_base.hpp
)

install(
    TARGETS
        ${LAZPERF_SHARED_LIB}
    EXPORT
        lazperf-targets
    LIBRARY DESTINATION lib
)

install(
    EXPORT
        lazperf-targets
    FILE
        lazperf-targets.cmake
    NAMESPACE
        LAZPERF::
    DESTINATION
        lib/cmake/LAZPERF
)

target_include_directories(${LAZPERF_SHARED_LIB}
    INTERFACE
        $<INSTALL_INTERFACE:include>)

#
# cmake file handling
#

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/lazperf-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/lazperf-config-version.cmake
    DESTINATION
        lib/cmake/LAZPERF
)
