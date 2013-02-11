(defun ct/find-decl ()
  (interactive)
  (save-excursion
    (forward-char)
    (backward-word)
    (let* ((offset (- (position-bytes (point)) 1))
          (command (format "../../clang-tags main.cxx %d -c -I. main.cxx" offset)))
      (switch-to-buffer-other-window (get-buffer-create "*clang-tags*"))
      (setq buffer-read-only nil)
      (erase-buffer)
      (insert command)
      (newline 2)
      (shell-command command t)
      (let ((compilation-error-regexp-alist
             '(("\\(.+\\):\\([[:digit:]]+\\)-\\([[:digit:]]+\\):\\([[:digit:]]+\\)-\\([[:digit:]]+\\)"
                1 (2 . 3) (4 . 5))
               ("\\(.+\\):\\([[:digit:]]+\\):\\([[:digit:]]+\\)"
                1 2 3))))
        (compilation-mode "clang-tags"))
      (goto-char (point-max))
      (compilation-previous-error 1))))

(defun ct/find-uses ()
  (interactive)
  (save-excursion
    (beginning-of-line)
    (search-forward "(")
    (let* ((beg (point))
           (usr (progn
                  (search-forward ")")
                  (buffer-substring beg (- (point) 1))))
           (output (get-buffer-create "*essai*")))
      (with-current-buffer output
        (setq buffer-read-only nil)
        (erase-buffer))
      (with-current-buffer ".clang-tags"
        (goto-char (point-min))
        (while (search-forward-regexp (concat "^" usr "\t") nil t)
          (let ((beg (point))
                (end (line-end-position)))
            (with-current-buffer output
              (insert-buffer-substring ".clang-tags" beg end)
              (newline)))))
      (switch-to-buffer output)
      (let ((compilation-error-regexp-alist
             '(("\\(.+\\):\\([[:digit:]]+\\)-\\([[:digit:]]+\\):\\([[:digit:]]+\\)-\\([[:digit:]]+\\)"
                1 (2 . 3) (4 . 5))
               ("\\(.+\\):\\([[:digit:]]+\\):\\([[:digit:]]+\\)"
                1 2 3))))
        (compilation-mode "clang-tags")))))