ct_push_dir (${CT_DIR}/sqlite++)

add_library (sqlite++
  ${CT_DIR}/database.cxx
  ${CT_DIR}/transaction.cxx
)
set (LIBS ${LIBS} sqlite++)

add_executable (test_sqlite++
  ${CT_DIR}/tests/test_sqlite++.cxx)
target_link_libraries (test_sqlite++ ${LIBS})
add_test (sqlite++ test_sqlite++)

ct_pop_dir ()