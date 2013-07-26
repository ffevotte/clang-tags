ct_push_dir (${CT_DIR}/util)

add_executable (test_util
  ${CT_DIR}/tests/test_util.cxx)
add_test (util test_util)

ct_pop_dir ()