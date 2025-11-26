;; -*- lexical-binding: t; -*-

(TeX-add-style-hook
 "regexp_8c_source"
 (lambda ()
   (setq TeX-command-extra-options
         "-shell-escape")
   (LaTeX-add-labels))
 :latex)

