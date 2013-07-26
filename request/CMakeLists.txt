ct_push_dir (${CT_DIR}/request)

add_executable (test_request
  ${CT_DIR}/tests/test_request.cxx)
target_link_libraries (test_request ${LIBS})
add_test (request test_request)

ct_pop_dir ()
