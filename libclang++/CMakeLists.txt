ct_push_dir (${CT_DIR}/libclang++)

add_library (clang++
  ${CT_DIR}/index.cxx
  ${CT_DIR}/translationUnit.cxx
  ${CT_DIR}/translationUnitCache.cxx
  ${CT_DIR}/sourceLocation.cxx
  ${CT_DIR}/cursor.cxx)
set (LIBS ${LIBS} clang++)

ct_pop_dir ()
