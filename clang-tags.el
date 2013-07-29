(require 'compile)

(defconst ct/source-location-re
  "\\(.+\\):\\([[:digit:]]+\\)-\\([[:digit:]]+\\):\\([[:digit:]]+\\)-\\([[:digit:]]+\\)")

(defvar ct/options ""
  "Command-line options for `clang-tags'")

;; (setq ct/options "")
;; (setq ct/options "--debug --most-specific")

(defvar ct/default-directory "."
  "Directory in which `clang-tags' shoud be run")



;;; Specific compilation mode for `clang-tags find-def'

(defvar ct/find-def-scroll-output 'first-error)

;; Error regexp
(defvar ct/find-def-error-regexp-alist
  `((,(concat "^   \\(" ct/source-location-re "\\)")
     2 (3 . 4) (5 . 6) 1 1)
    ("^\\(.+\\):\\([[:digit:]]+\\):\\([[:digit:]]+\\): "
     1 2 3 1 1))
  "Regular expressions used to match source code locations in clang-tags' output.
See `compilation-error-regexp-alist'.")

(define-compilation-mode ct/find-def-mode "Clang-Tags/find-def"
  "clang-tags - find definition mode

This mode is a custom compilation mode to display the results of
a `clang-tags find-def' command. Here are the most useful
bindings in this mode: \\<ct/find-def-mode-map>

\\[compile-goto-error] 	 go to the location of the definition at point
\\[ct/grep-tag] 	 find in the project all uses of the definition at point


Below is the complete keymap, including standard bindings
inherited from `compilation-mode':

\\{ct/find-def-mode-map}")

;; Keymap
(define-key ct/find-def-mode-map (kbd "M-,") 'ct/grep-tag)

;; Syntax highlighting
(font-lock-add-keywords
   'ct/find-def-mode
   '(("^-- \\(.*\\) -- " 1 font-lock-keyword-face)))



;;; Front-end for `clang-tags find-def'

(defun ct/find-def (argp)
  "Find the definition of the symbol under point

This function uses `clang-tags' to find the location of the
definition(s) of the symbol under point. Results are presented in a
buffer which allows quickly navigating to those locations."
  (interactive "P")
  (let* ((offset (- (position-bytes (point)) 1))
         (default-directory ct/default-directory)
         (fromIndex (if argp "-r" "-i"))
         (command (format "cd %s;\ntime clang-tags find-def %s %s %d %s\n"
                          ct/default-directory
                          fromIndex
                          buffer-file-name offset ct/options)))

    (switch-to-buffer (get-buffer-create "*ct/find-def*"))
    (compilation-start command
                       'ct/find-def-mode)))



;;; Front-end for `clang-tags grep'

(defun ct/grep-tag (usr)
  "Find in the code base all uses of the definition at point.

This function uses `clang-tags' to find all uses of the current
definition, as identified by its clang USR (Unified Symbol
Resolution). Results are presented in a `grep-mode' buffer, which
allows benefiting from the full power of Emacs. In particular,
`wgrep-change-to-wgrep-mode' might be useful for refactoring
operations."
  (interactive `(,(save-excursion
                    (search-forward "USR: ")
                    (buffer-substring (point) (line-end-position)))))
  (message "%s" usr)
  (switch-to-buffer (get-buffer-create "*ct/grep*"))
  (compilation-start (format "clang-tags grep '%s' | sort -u" usr)
                     'grep-mode
                     (lambda (mode) "" "*ct/grep*")))



;;; Clang-tags minor mode
(define-minor-mode clang-tags-mode
  "\\{clang-tags-mode-map}"
  :lighter " ct"
  :keymap (let ((map (make-sparse-keymap)))
            (define-key map (kbd "M-.") 'ct/find-def)
            map))

(provide 'clang-tags)