ct_push_dir (${CT_DIR}/getopt++)

add_library (getopt++
  ${CT_DIR}/getopt.cxx)
set (LIBS ${LIBS} getopt++)

ct_pop_dir ()