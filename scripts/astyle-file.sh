#!/bin/bash

filename=$1

if [[ -n "$filename" ]]; then
astyle \
--style=allman \
--indent=tab \
--indent-switches \
--indent-cases \
--indent-namespaces \
--indent-labels \
--indent-preproc-block \
--indent-preproc-define \
--indent-col1-comments \
--min-conditional-indent=0 \
--max-instatement-indent=80 \
--break-blocks \
--pad-oper \
--pad-paren-in \
--unpad-paren \
--align-pointer=name \
--align-reference=name \
--break-closing-brackets \
--add-brackets \
--keep-one-line-statements \
--close-templates \
--remove-comment-prefix \
--max-code-length=200 \
--suffix=none \
--lineend=linux \
$filename
else
    echo "argument error"
fi