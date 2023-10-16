-- Config file for ldoc
project = "lua-pdfgen"
description = "Lua binding library for PDFGen"
dir = "docs" -- for compatibility with github pages

format='discount'
-- backtick_references=false
file={
    '../lua-pdfgen.c',
    '../compat.h'
}
readme='../README.md'
style='!pale'
kind_names={topic='Manual',script='Programs'}
examples = {
    '../examples/main.lua',
    '../examples/massive_file.lua'
}
