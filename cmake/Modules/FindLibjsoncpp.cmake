# Check for jsoncpp
find_package (PkgConfig)
pkg_check_modules (PC_JSONCPP QUIET jsoncpp)

find_path (Libjsoncpp_INCLUDE_DIR json/json.h
  HINTS ${PC_JSONCPP_INCLUDEDIR} ${PC_JSONCPP_INCLUDE_DIRS}
  PATH "${LIBJSONCPP_ROOT}/include")

find_library (Libjsoncpp_LIBRARY jsoncpp
  HINTS ${PC_JSONCPP_LIBDIR} ${PC_JSONCPP_LIBRARY_DIRS}
  PATH "${LIBJSONCPP_ROOT}/lib")

set (Libjsoncpp_LIBRARIES    ${Libjsoncpp_LIBRARY} )
set (Libjsoncpp_INCLUDE_DIRS ${Libjsoncpp_INCLUDE_DIR} )

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (LibJsoncpp  DEFAULT_MSG
  Libjsoncpp_LIBRARY Libjsoncpp_INCLUDE_DIR)

mark_as_advanced (Libjsoncpp_INCLUDE_DIR Libjsoncpp_LIBRARY)