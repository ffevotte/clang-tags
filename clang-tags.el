(defconst ct/source-location-re
  "\\(.+\\):\\([[:digit:]]+\\)-\\([[:digit:]]+\\):\\([[:digit:]]+\\)-\\([[:digit:]]+\\)")

(defvar ct/options ""
  "Command-line options for `clang-tags'")

;; (setq ct/options "")
;; (setq ct/options "--debug --most-specific")

(defvar ct/default-directory "."
  "Directory in which `clang-tags' shoud be run")

(defun ct/find-decl ()
  (interactive)
  (let* ((offset (- (position-bytes (point)) 1))
         (default-directory ct/default-directory)
         (command (format "time clang-tags find-def %s %d %s"
                          buffer-file-name offset ct/options))
         (compilation-error-regexp-alist
          `((,(concat "^   \\(" ct/source-location-re "\\)")
             2 (3 . 4) (5 . 6) 1 1)
            ("^\\(.+\\):\\([[:digit:]]+\\):\\([[:digit:]]+\\): "
             1 2 3 1 1)))

         (output-buffer "*clang-tags find-def*"))

    (switch-to-buffer (get-buffer-create output-buffer))
    (compilation-start command
                       nil
                       (lambda (mode) "" output-buffer))

    (with-current-buffer output-buffer

      ;; Go to the last message after compilation is done
      (set (make-local-variable 'compilation-finish-functions)
           `(,(lambda (buffer message)
                (select-window (get-buffer-window buffer))
                (goto-char (point-max))
                (compilation-previous-error 1))))

      ;; Font-locking
      (font-lock-add-keywords
       nil
       '(("^-- \\(.*\\) -- " 1 font-lock-keyword-face)))
      (font-lock-fontify-buffer))))

(defun ct/grep-tag (usr)
  (interactive `(,(save-excursion
                    (search-forward "USR: ")
                    (buffer-substring (point) (line-end-position)))))
  (message "%s" usr)
  (compilation-start (format "clang-tags grep '%s' | sort -u" usr)
                       'grep-mode
                       (lambda (mode) "" "*clang-tags grep-tag*")))




(global-set-key (kbd "M-.") 'ct/find-decl)

(with-temp-buffer
  (shell-command (concat
                  "DIR=/home/francois/projets/git/clang-tags/build/\n"
                  "if echo \"$PATH\" | grep -q \":$DIR:\"; then\n"
                  "  echo -n $PATH\n"
                  "else\n"
                  "  echo -n \"$PATH:$DIR:\"\n"
                  "fi")
                 t)
  (setenv "PATH" (buffer-substring (point-min) (point-max))))

(with-temp-buffer
  (shell-command (concat
                  "DIR=/home/francois/projets/git/clang-tags/src/\n"
                  "if echo \"$PATH\" | grep -q \":$DIR:\"; then\n"
                  "  echo -n $PATH\n"
                  "else\n"
                  "  echo -n \"$PATH:$DIR:\"\n"
                  "fi")
                 t)
  (setenv "PATH" (buffer-substring (point-min) (point-max))))
