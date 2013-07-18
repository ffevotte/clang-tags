# Check for jsoncpp
find_path (LIBJSONCPP_INCLUDE_DIR json/json.h
  PATH "/usr/include/jsoncpp")

find_library (LIBJSONCPP_LIBRARY jsoncpp)

set (LIBJSONCPP_LIBRARIES    ${LIBJSONCPP_LIBRARY} )
set (LIBJSONCPP_INCLUDE_DIRS ${LIBJSONCPP_INCLUDE_DIR} )

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (LibJsoncpp  DEFAULT_MSG
  LIBJSONCPP_LIBRARY LIBJSONCPP_INCLUDE_DIR)

mark_as_advanced (LIBJSONCPP_INCLUDE_DIR LIBJSONCPP_LIBRARY)