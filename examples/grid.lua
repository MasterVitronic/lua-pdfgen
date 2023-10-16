#!/usr/bin/env lua

--[[
 @filename  grid.lua
 @version   1.0
 @autor     Máster Vitronic <mastervitronic@gmail.com>
 @date      lun oct 16 09:50:42 -04 2023
 @licence   MIT licence
]]--

local pdfgen = require("pdfgen")
local pdf = pdfgen:new()

PDF_LETTER_WIDTH  =  612.0
PDF_LETTER_HEIGHT = 792.0
local options = {
  creator = 'Vitronic',
  producer= 'producer',
  title   = 'title',
  author  = 'author',
  subject = 'subject',
  date    = os.date('%Y%m%d%H%M%SZ')
}

pdf:create(
  PDF_LETTER_WIDTH,
  PDF_LETTER_HEIGHT,
  options
)

local page = pdf:append_page()
pdf:set_font('Times-Roman')

local black = pdfgen.rgb(0,0,0)
local blue  = pdfgen.rgb(0,0,255)
local red   = pdfgen.rgb(255,0,0)

--create grid
for i = 0, pdf:width(), 10 do
  pdf:add_line(nil, i, 0, i, pdf:height(), 0.1, black);
  pdf:add_text_rotate(nil, i, 4, i-1, 1, 1.57, blue);
end

for i = 0, pdf:height(),10 do
  pdf:add_line(nil, 0, i, pdf:width(), i, 0.1, black);
  pdf:add_text_rotate(nil, i, 4, 1,i+1, 0, blue);
end
pdf:add_rectangle(nil, 40, 160, 40, 40, 1, red);

pdf:save("out.pdf");
pdf:destroy();
