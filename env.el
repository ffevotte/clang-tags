(let ((path (getenv "PATH")))
  (setenv "PATH" (concat path ":@PROJECT_SOURCE_DIR@:@PROJECT_BINARY_DIR@")))

(load-file "@PROJECT_SOURCE_DIR@/clang-tags.el")