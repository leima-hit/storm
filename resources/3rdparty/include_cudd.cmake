
####
#### Find autoreconf for cudd update step
find_program(AUTORECONF autoreconf)
find_program(ACLOCAL aclocal)
mark_as_advanced(AUTORECONF)
mark_as_advanced(ACLOCAL)

if (NOT AUTORECONF)
    message(FATAL_ERROR "Cannot find autoreconf, cannot compile cudd3.")
endif()

if (NOT ACLOCAL)
    message(FATAL_ERROR "Cannot find aclocal, cannot compile cudd3.")
endif()

set(CUDD_LIB_DIR ${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0/lib)

set(STORM_CUDD_FLAGS "CFLAGS=-O3 -w -DPIC -DHAVE_IEEE_754 -fno-common -ffast-math -fno-finite-math-only")
if (NOT STORM_PORTABLE)
	set(STORM_CUDD_FLAGS "${STORM_CUDD_FLAGS} -march=native")
endif()

ExternalProject_Add(
        cudd3
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/cudd-3.0.0
        PREFIX ${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0
        PATCH_COMMAND ${AUTORECONF}
        CONFIGURE_COMMAND ${STORM_3RDPARTY_SOURCE_DIR}/cudd-3.0.0/configure --enable-shared --enable-obj --with-pic=yes --prefix=${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0 --libdir=${CUDD_LIB_DIR} CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
        BUILD_COMMAND make ${STORM_CUDD_FLAGS}
        INSTALL_COMMAND make install
        BUILD_IN_SOURCE 0
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
        BUILD_BYPRODUCTS ${CUDD_LIB_DIR}/libcudd${DYNAMIC_EXT} ${CUDD_LIB_DIR}/libcudd${STATIC_EXT}
)

# Do not use system CUDD, StoRM has a modified version
set(CUDD_INCLUDE_DIR ${STORM_3RDPARTY_BINARY_DIR}/cudd-3.0.0/include)
set(CUDD_SHARED_LIBRARY ${CUDD_LIB_DIR}/libcudd${DYNAMIC_EXT})
set(CUDD_STATIC_LIBRARY ${CUDD_LIB_DIR}/libcudd${STATIC_EXT})
set(CUDD_VERSION_STRING 3.0.0)

add_imported_library(cudd SHARED ${CUDD_SHARED_LIBRARY} ${CUDD_INCLUDE_DIR})
add_imported_library(cudd STATIC ${CUDD_STATIC_LIBRARY} ${CUDD_INCLUDE_DIR})

add_dependencies(resources cudd3)

if(BUILD_SHARED_LIBS)
	list(APPEND STORM_DEP_TARGETS cudd_SHARED)
else()
    list(APPEND STORM_DEP_TARGETS cudd_STATIC)
endif()

message(STATUS "storm - Linking with CUDD ${CUDD_VERSION_STRING}.")
