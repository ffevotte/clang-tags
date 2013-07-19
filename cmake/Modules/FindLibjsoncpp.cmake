# Check for jsoncpp
find_path (Libjsoncpp_INCLUDE_DIR json/json.h
  PATH "/usr/include/jsoncpp"
  HINTS "${LIBJSONCPP_ROOT}/include")

find_library (Libjsoncpp_LIBRARY jsoncpp
  HINTS "${LIBJSONCPP_ROOT}/lib")

set (Libjsoncpp_LIBRARIES    ${Libjsoncpp_LIBRARY} )
set (Libjsoncpp_INCLUDE_DIRS ${Libjsoncpp_INCLUDE_DIR} )

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (LibJsoncpp  DEFAULT_MSG
  Libjsoncpp_LIBRARY Libjsoncpp_INCLUDE_DIR)

mark_as_advanced (Libjsoncpp_INCLUDE_DIR Libjsoncpp_LIBRARY)