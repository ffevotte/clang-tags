find_path (Libsqlite3_INCLUDE_DIR sqlite3.h)
find_library (Libsqlite3_LIBRARY sqlite3)

set (Libsqlite3_LIBRARIES    ${Libsqlite3_LIBRARY} )
set (Libsqlite3_INCLUDE_DIRS ${Libsqlite3_INCLUDE_DIR} )

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Libsqlite3  DEFAULT_MSG
  Libsqlite3_LIBRARY Libsqlite3_INCLUDE_DIR)

mark_as_advanced (Libsqlite3_INCLUDE_DIR Libsqlite3_LIBRARY)