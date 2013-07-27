(defmacro create-read-only-derived-mode (mode-name)
  `(define-derived-mode ,(intern (format "%s-rw-mode" mode-name)) ,(intern (format "%s-mode" mode-name))
     ,(format "%s-rw-mode" mode-name)
     ,(format "Like %s-mode, but not read-only" mode-name)
     (setq buffer-read-only nil)))

(create-read-only-derived-mode "grep")
(create-read-only-derived-mode "ct/find-def")
