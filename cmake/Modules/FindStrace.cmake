find_program (Strace_PROGRAM strace)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Strace  DEFAULT_MSG
  Strace_PROGRAM)

mark_as_advanced (Strace_PROGRAM)