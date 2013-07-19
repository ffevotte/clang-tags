find_path (Libclang_INCLUDE_DIR clang-c/Index.h
  HINTS ${LIBCLANG_ROOT})
find_library (Libclang_LIBRARY clang
  HINTS ${LIBCLANG_ROOT})

set (Libclang_LIBRARIES    ${Libclang_LIBRARY})
set (Libclang_INCLUDE_DIRS ${Libclang_INCLUDE_DIR})

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Libclang  DEFAULT_MSG
  Libclang_LIBRARY Libclang_INCLUDE_DIR)

mark_as_advanced (Libclang_INCLUDE_DIR Libclang_LIBRARY)