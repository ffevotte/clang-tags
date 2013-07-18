find_program (Socat_PROGRAM socat)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Socat  DEFAULT_MSG
  Socat_PROGRAM)

mark_as_advanced (Socat_PROGRAM)