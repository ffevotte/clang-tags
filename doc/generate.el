
;; Load environment
(load-file "@PROJECT_BINARY_DIR@/env.el")

;; Create non-read-only variants of clang-tags modes
(defmacro create-read-only-derived-mode (mode-name)
  `(define-derived-mode ,(intern (format "%s-rw-mode" mode-name)) ,(intern (format "%s-mode" mode-name))
     ,(format "%s-rw-mode" mode-name)
     ,(format "Like %s-mode, but not read-only" mode-name)
     (setq buffer-read-only nil)))
(create-read-only-derived-mode "grep")
(create-read-only-derived-mode "ct/find-def")

;; Export org-mode files
(mapc (lambda (fileName)
	(find-file fileName)
        (org-html-export-to-html))
      '("index.org" "quickStart.org" "install.org"))

(kill-emacs)