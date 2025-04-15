# FindGraphicsMagick.cmake
# - Find GraphicsMagick
# Find the GraphicsMagick includes and library
#
#  GraphicsMagick_INCLUDE_DIRS - Where to find GraphicsMagick headers
#  GraphicsMagick_LIBRARIES    - List of libraries when using GraphicsMagick
#  GraphicsMagick_FOUND        - True if GraphicsMagick is found

find_package(PkgConfig QUIET)
pkg_check_modules(PC_GraphicsMagick QUIET GraphicsMagick)

find_path(GraphicsMagick_INCLUDE_DIR
        NAMES magick/api.h
        HINTS ${PC_GraphicsMagick_INCLUDEDIR} ${PC_GraphicsMagick_INCLUDE_DIRS}
        PATH_SUFFIXES GraphicsMagick
)

find_library(GraphicsMagick_LIBRARY
        NAMES GraphicsMagick
        HINTS ${PC_GraphicsMagick_LIBDIR} ${PC_GraphicsMagick_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GraphicsMagick DEFAULT_MSG
        GraphicsMagick_LIBRARY GraphicsMagick_INCLUDE_DIR)

if(GraphicsMagick_FOUND)
    set(GraphicsMagick_LIBRARIES ${GraphicsMagick_LIBRARY})
    set(GraphicsMagick_INCLUDE_DIRS ${GraphicsMagick_INCLUDE_DIR})

    if(NOT TARGET GraphicsMagick::GraphicsMagick)
        add_library(GraphicsMagick::GraphicsMagick UNKNOWN IMPORTED)
        set_target_properties(GraphicsMagick::GraphicsMagick PROPERTIES
                IMPORTED_LOCATION "${GraphicsMagick_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${GraphicsMagick_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(GraphicsMagick_INCLUDE_DIR GraphicsMagick_LIBRARY)