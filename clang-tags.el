(defconst ct/source-location-re
  "\\(.+\\):\\([[:digit:]]+\\)-\\([[:digit:]]+\\):\\([[:digit:]]+\\)-\\([[:digit:]]+\\)")

(defvar ct/options ""
  "Command-line options for `clang-tags'")

;; (setq ct/options "")
;; (setq ct/options "--debug --most-specific")

(defun ct/find-decl ()
  (interactive)
  (save-excursion
    (c-beginning-of-current-token)
    (let* ((offset (- (position-bytes (point)) 1))
           (command (format "time clang-tags '<stdin>' %d %s -- -x c++ -std=c++11 -Wall -c -I . -" offset ct/options))
           (origin  (current-buffer))
           (output  (get-buffer-create (concat "*clang-tags: " (buffer-name) "*"))))

      (let ((resize-mini-windows nil))
        (shell-command-on-region (point-min) (point-max) command output))
      (switch-to-buffer-other-window output)
      (goto-char (point-min))
      (insert command)
      (newline 2)

      ;; Replace references by the buffer substring
      (while (search-forward-regexp
              (concat "^-- " ct/source-location-re)
              nil t)
        (let ((file (match-string 1))
              (line1 (read (match-string 2)))
              (line2 (read (match-string 3)))
              (col1  (read (match-string 4)))
              (col2  (read (match-string 5)))
              beg end expr)
          (with-current-buffer origin
            (save-excursion
              (goto-char (point-min))
              (forward-line (- line1 1))
              (forward-char (- col1 1))
              (setq beg (point))
              (goto-char (point-min))
              (forward-line (- line2 1))
              (forward-char col2)
              (setq end (+ 30 beg))
              (if (<= (point) end)
                  (setq expr (buffer-substring beg (point)))
                (setq expr (concat (buffer-substring beg end) "...")))))
          (forward-line 0)
          (kill-line)
          (let ((beg (point))
                end)
            (insert "-- " expr " -- ")
            (save-excursion
              (setq end (point))
              (goto-char beg)
              (while (search-forward "\n" end t)
                (replace-match " "))))
          (kill-line)))

      ;; Replace <stdin> with the current file name
      (goto-char (point-min))
      (while (re-search-forward "^\\([[:space:]]*\\)<stdin>:" nil t)
        (replace-match (concat "\\1"
                               (file-name-nondirectory (buffer-file-name origin))
                               ":")))

      ;; Switch to `compilation-mode'
      (let ((compilation-error-regexp-alist
             `((,(concat "^   \\(" ct/source-location-re "\\)")
                2 (3 . 4) (5 . 6) 1 1)
               ("^\\(.+\\):\\([[:digit:]]+\\):\\([[:digit:]]+\\)"
                1 2 3 1 1))))
        (compilation-mode "clang-tags"))

      ;; Font-locking
      (font-lock-add-keywords
       nil
       '(("^-- \\(.*\\) -- " 1 font-lock-keyword-face)))
      (font-lock-fontify-buffer)

      ;; Select last compilation error
      (goto-char (point-max))
      (compilation-previous-error 1)
      ;(compile-goto-error)
      )))

(global-set-key (kbd "M-.") 'ct/find-decl)

(with-temp-buffer
  (shell-command (concat "if echo \"$PATH\" | grep -q \":$PWD:\"; then\n"
                         "  echo -n $PATH\n"
                         "else\n"
                         "  echo -n \"$PATH:$PWD:\"\n"
                         "fi")
                 t)
  (setenv "PATH" (buffer-substring (point-min) (point-max))))